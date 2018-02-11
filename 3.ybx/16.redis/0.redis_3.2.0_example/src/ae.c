#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "ae.h"
#include "zmalloc.h"
#include "config.h"

#ifdef HAVE_EVPORT
#include "ae_evport.c"
#else
    #ifdef HAVE_EPOLL
    #include "ae_epoll.c"
    #else
        #ifdef HAVE_KQUEUE
        #include "ae_kqueue.c"
        #else
        #include "ae_select.c"
        #endif
    #endif
#endif

// 创建并初始化一个事件轮询的状态结构
aeEventLoop *aeCreateEventLoop(int setsize) {
	aeEventLoop *eventLoop;
	int i;

	// 分配事件轮询的状态结构空间
	if ((eventLoop = zmalloc(sizeof(*eventLoop))) == NULL) goto err;
	// 分配文件事件表和就绪事件表空间
	eventLoop->events = zmalloc(sizeof(aeFileEvent)*setsize);
	eventLoop->fired = zmalloc(sizeof(aeFiredEvent)*setsize);
	if (eventLoop->events == NULL || eventLoop->fired == NULL) goto err;
	// 设置事件表的大小
	eventLoop->setsize = setsize;
	// 设置最近一次执行的时间
	eventLoop->lastTime = time(NULL);
	// 初始化时间事件
	eventLoop->timeEventHead = NULL;
	eventLoop->timeEventNextId = 0;
	// 事件处理打开
	eventLoop->stop = 0;
	eventLoop->maxfd = -1;
	eventLoop->beforesleep = NULL;
	// 创建一个epoll实例保存到eventLoop的events中
	if (aeApiCreate(eventLoop) == -1) goto err;
	/* Events with mask == AE_NONE are not set. So let's initialize the
	 * vector with it. */
	// 初始化监听的事件为空
	for (i = 0; i < setsize; i++)
		eventLoop->events[i].mask = AE_NONE;
	// 返回事件轮询的状态结构的地址
	return eventLoop;

	// 出错处理：释放空间
err:
	if (eventLoop) {
		zfree(eventLoop->events);
		zfree(eventLoop->fired);
		zfree(eventLoop);
	}
	return NULL;
}

int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData)
{
    if (fd >= eventLoop->setsize) {
        errno = ERANGE;
        return AE_ERR;
    }
    aeFileEvent *fe = &eventLoop->events[fd];

    if (aeApiAddEvent(eventLoop, fd, mask) == -1)
        return AE_ERR;
    fe->mask |= mask;
    if (mask & AE_READABLE) fe->rfileProc = proc;
    if (mask & AE_WRITABLE) fe->wfileProc = proc;
    fe->clientData = clientData;
    if (fd > eventLoop->maxfd)
        eventLoop->maxfd = fd;
    return AE_OK;
}


void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask)
{
    if (fd >= eventLoop->setsize) return;
    aeFileEvent *fe = &eventLoop->events[fd];
    if (fe->mask == AE_NONE) return;

    aeApiDelEvent(eventLoop, fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == eventLoop->maxfd && fe->mask == AE_NONE) {
        /* Update the max fd */
        int j;

        for (j = eventLoop->maxfd-1; j >= 0; j--)
            if (eventLoop->events[j].mask != AE_NONE) break;
        eventLoop->maxfd = j;
    }
}


// 获取fd的事件类型
int aeGetFileEvents(aeEventLoop *eventLoop, int fd) {
    // 越界，直接返回
    if (fd >= eventLoop->setsize) return 0;
    aeFileEvent *fe = &eventLoop->events[fd];

    return fe->mask;
}




void aeDeleteEventLoop(aeEventLoop *eventLoop) {
    aeApiFree(eventLoop);
    zfree(eventLoop->events);
    zfree(eventLoop->fired);
    zfree(eventLoop);
}


// 获取当前时间秒和毫秒，并保存到参数中
static void aeGetTime(long *seconds, long *milliseconds)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}

// 将milliseconds个毫秒转换为sec个秒和ms个毫秒，保存到参数中
static void aeAddMillisecondsToNow(long long milliseconds, long *sec, long *ms) {
    long cur_sec, cur_ms, when_sec, when_ms;

    // 获取当前的时间
    aeGetTime(&cur_sec, &cur_ms);
    // 获取秒数
    when_sec = cur_sec + milliseconds/1000;
    // 获取毫秒数
    when_ms = cur_ms + milliseconds%1000;
    // 毫秒数大于1000需要进位
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }
    // 保存秒数和毫秒数
    *sec = when_sec;
    *ms = when_ms;
}

long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
		aeTimeProc *proc, void *clientData,
		aeEventFinalizerProc *finalizerProc)
{
	long long id = eventLoop->timeEventNextId++;
	aeTimeEvent *te;

	te = zmalloc(sizeof(*te));
	if (te == NULL) return AE_ERR;
	te->id = id;
	aeAddMillisecondsToNow(milliseconds,&te->when_sec,&te->when_ms);
	te->timeProc = proc;
	te->finalizerProc = finalizerProc;
	te->clientData = clientData;
	te->next = eventLoop->timeEventHead;
	eventLoop->timeEventHead = te;
}

// 寻找第一个快到时的时间事件
// 这个操作是有用的知道有多少时间可以选择该事件设置为不用推迟任何事件的睡眠中。
// 如果事件链表没有时间将返回NULL。
static aeTimeEvent *aeSearchNearestTimer(aeEventLoop *eventLoop)
{
	// 时间事件头节点地址
	aeTimeEvent *te = eventLoop->timeEventHead;
	aeTimeEvent *nearest = NULL;

	// 遍历所有的时间事件
	while(te) {
		// 寻找第一个快到时的时间事件，保存到nearest中
		if (!nearest || te->when_sec < nearest->when_sec ||
				(te->when_sec == nearest->when_sec &&
				 te->when_ms < nearest->when_ms))
			nearest = te;
		te = te->next;
	}
	return nearest;
}

/* Process time events */
// 执行时间事件
static int processTimeEvents(aeEventLoop *eventLoop) {
	int processed = 0;
	aeTimeEvent *te, *prev;
	long long maxId;
	time_t now = time(NULL);

	/* If the system clock is moved to the future, and then set back to the
	 * right value, time events may be delayed in a random way. Often this
	 * means that scheduled operations will not be performed soon enough.
	 *
	 * Here we try to detect system clock skews, and force all the time
	 * events to be processed ASAP when this happens: the idea is that
	 * processing events earlier is less dangerous than delaying them
	 * indefinitely, and practice suggests it is. */
	// 这里尝试发现时间混乱的情况，上一次处理事件的时间比当前时间还要大
	// 重置最近一次处理事件的时间
	if (now < eventLoop->lastTime) {
		te = eventLoop->timeEventHead;
		while(te) {
			te->when_sec = 0;
			te = te->next;
		}
	}
	// 设置上一次时间事件处理的时间为当前时间
	eventLoop->lastTime = now;

	prev = NULL;
	te = eventLoop->timeEventHead;
	maxId = eventLoop->timeEventNextId-1;   //当前时间事件表中的最大ID
	// 遍历时间事件链表
	while(te) {
		long now_sec, now_ms;
		long long id;

		/* Remove events scheduled for deletion. */
		// 如果时间事件已被删除了
		if (te->id == AE_DELETED_EVENT_ID) {
			aeTimeEvent *next = te->next;
			// 从事件链表中删除事件的节点
			if (prev == NULL)
				eventLoop->timeEventHead = te->next;
			else
				prev->next = te->next;
			// 调用时间事件终结方法清楚该事件
			if (te->finalizerProc)
				te->finalizerProc(eventLoop, te->clientData);
			zfree(te);
			te = next;
			continue;
		}

		/* Make sure we don't process time events created by time events in
		 * this iteration. Note that this check is currently useless: we always
		 * add new timers on the head, however if we change the implementation
		 * detail, this check may be useful again: we keep it here for future
		 * defense. */
		// 确保我们不处理在此迭代中由时间事件创建的时间事件。 
		// 请注意，此检查目前无效：我们总是在头节点添加新的计时器，但是如果我们更改实施细节，
		// 则该检查可能会再次有用：我们将其保留在未来的防御
		if (te->id > maxId) {
			te = te->next;
			continue;
		}
		// 获取当前时间
		aeGetTime(&now_sec, &now_ms);
		// 找到已经到时的时间事件
		if (now_sec > te->when_sec ||
				(now_sec == te->when_sec && now_ms >= te->when_ms))
		{
			int retval;

			id = te->id;
			// 调用时间事件处理方法
			retval = te->timeProc(eventLoop, id, te->clientData);
			// 时间事件次数加1
			processed++;
			// 如果不是定时事件，则继续设置它的到时时间
			if (retval != AE_NOMORE) {
				aeAddMillisecondsToNow(retval,&te->when_sec,&te->when_ms);
				// 如果是定时时间，则retval为-1，则将其时间事件删除，惰性删除
			} else {
				te->id = AE_DELETED_EVENT_ID;
			}
		}
		// 更新前驱节点指针和后继节点指针
		prev = te;
		te = te->next;
	}
	return processed;   //返回执行事件的次数
}//end of static int processTimeEvents(aeEventLoop *eventLoop);

/* Process every pending time event, then every pending file event
 * (that may be registered by time event callbacks just processed).
 * Without special flags the function sleeps until some file event
 * fires, or when the next time event occurs (if any).
 *
 * If flags is 0, the function does nothing and returns.
 * if flags has AE_ALL_EVENTS set, all the kind of events are processed.
 * if flags has AE_FILE_EVENTS set, file events are processed.
 * if flags has AE_TIME_EVENTS set, time events are processed.
 * if flags has AE_DONT_WAIT set the function returns ASAP until all
 * the events that's possible to process without to wait are processed.
 *
 * The function returns the number of events processed. */
// 处理到时的时间事件和就绪的文件事件
// 如果flags = 0，函数什么都不做，直接返回
// 如果flags设置了 AE_ALL_EVENTS ，则执行所有类型的事件
// 如果flags设置了 AE_FILE_EVENTS ，则执行文件事件
// 如果flags设置了 AE_TIME_EVENTS ，则执行时间事件
// 如果flags设置了 AE_DONT_WAIT ，那么函数处理完事件后直接返回，不阻塞等待
// 函数返回执行的事件个数
int aeProcessEvents(aeEventLoop *eventLoop, int flags)
{
	int processed = 0, numevents;

	/* Nothing to do? return ASAP */
	// 如果什么事件都没有设置则直接返回
	if (!(flags & AE_TIME_EVENTS) && !(flags & AE_FILE_EVENTS)) return 0;

	/* Note that we want call select() even if there are no
	 * file events to process as long as we want to process time
	 * events, in order to sleep until the next time event is ready
	 * to fire. */
	// 请注意，既然我们要处理时间事件，即使没有要处理的文件事件，我们仍要调用select（），以便在下一次事件准备启动之前进行休眠

	// 当前还没有要处理的文件事件，或者设置了时间时间但是没有设置不阻塞标识
	if (eventLoop->maxfd != -1 ||
			((flags & AE_TIME_EVENTS) && !(flags & AE_DONT_WAIT))) {
		int j;
		aeTimeEvent *shortest = NULL;
		struct timeval tv, *tvp;

		// 如果设置了时间事件而没有设置不阻塞标识
		if (flags & AE_TIME_EVENTS && !(flags & AE_DONT_WAIT))
			// 获取最近到时的时间事件
			shortest = aeSearchNearestTimer(eventLoop);
		// 获取到了最早到时的时间事件
		if (shortest) {
			long now_sec, now_ms;
			// 获取当前时间
			aeGetTime(&now_sec, &now_ms);
			tvp = &tv;

			/* How many milliseconds we need to wait for the next
			 * time event to fire? */
			// 等待该时间事件到时所需要的时长
			long long ms =
				(shortest->when_sec - now_sec)*1000 +
				shortest->when_ms - now_ms;

			// 如果没到时
			if (ms > 0) {
				// 保存时长到tvp中
				tvp->tv_sec = ms/1000;
				tvp->tv_usec = (ms % 1000)*1000;
				// 如果已经到时，则将tvp的时间设置为0
			} else {
				tvp->tv_sec = 0;
				tvp->tv_usec = 0;
			}

			// 没有获取到了最早到时的时间事件，时间事件链表为空
		} else {
			/* If we have to check for events but need to return
			 * ASAP because of AE_DONT_WAIT we need to set the timeout
			 * to zero */
			// 如果设置了不阻塞标识
			if (flags & AE_DONT_WAIT) {
				// 将tvp的时间设置为0，就不会阻塞
				tv.tv_sec = tv.tv_usec = 0;
				tvp = &tv;
			} else {
				// 阻塞到第一个时间事件的到来
				/* Otherwise we can block */
				tvp = NULL; /* wait forever */
			}
		}

		// 等待所监听文件描述符上有事件发生
		// 如果tvp为NULL，则阻塞在此，否则等待tvp设置阻塞的时间，就会有时间事件到时
		// 返回了就绪文件事件的个数
		numevents = aeApiPoll(eventLoop, tvp);
		// 遍历就绪文件事件表
		for (j = 0; j < numevents; j++) {
			// 获取就绪文件事件的地址
			aeFileEvent *fe = &eventLoop->events[eventLoop->fired[j].fd];
			// 获取就绪文件事件的类型，文件描述符
			int mask = eventLoop->fired[j].mask;
			int fd = eventLoop->fired[j].fd;
			int rfired = 0;

			/* note the fe->mask & mask & ... code: maybe an already processed
			 * event removed an element that fired and we still didn't
			 * processed, so we check if the event is still valid. */
			// 如果是文件可读事件发生
			if (fe->mask & mask & AE_READABLE) {
				// 设置读事件标识 且 调用读事件方法处理读事件
				rfired = 1;
				fe->rfileProc(eventLoop,fd,fe->clientData,mask);
			}
			// 如果是文件可写事件发生
			if (fe->mask & mask & AE_WRITABLE) {
				// 读写事件的执行发法不同，则执行写事件，避免重复执行相同的方法
				if (!rfired || fe->wfileProc != fe->rfileProc)
					fe->wfileProc(eventLoop,fd,fe->clientData,mask);
			}
			processed++;    //执行的事件次数加1
		}
	}
	/* Check time events */
	// 执行时间事件
	if (flags & AE_TIME_EVENTS)
		processed += processTimeEvents(eventLoop);

	return processed; /* return the number of processed file/time events */
}

void aeMain(aeEventLoop *eventLoop) {
	eventLoop->stop = 0;
	while (!eventLoop->stop) {
		if (eventLoop->beforesleep != NULL)
			eventLoop->beforesleep(eventLoop);
		aeProcessEvents(eventLoop, AE_ALL_EVENTS);
	}
}

// 设置处理事件之前的函数
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep) {
    eventLoop->beforesleep = beforesleep;
}
