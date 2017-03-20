#include <sys/types.h>

#include <event3/event-config.h>

#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _EVENT_HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#endif
#include <errno.h>

#include  <event3/util.h>
#include  <event3/buffer.h>
#include  <event3/buffer_compat.h>
#include  <event3/bufferevent.h>
#include  <event3/bufferevent_struct.h>
#include  <event3/bufferevent_compat.h>
#include  <event3/event.h>
#include "log-internal.h"
#include "mm-internal.h"
#include "bufferevent-internal.h"
#include "evbuffer-internal.h"
#include "util-internal.h"

static void _bufferevent_cancel_all(struct bufferevent *bev);

//处理读事件：
//底层的socket fd接收数据后，bufferevent是怎么工作的。


//水位有2中情况读水位和写水位.
//这里只讲解读事件的水位。

//读事件的水位：
//在讲解读事件之前，先来看一下水位问题，函数 bufferevent_setwatermark 可以设置读和写的水位。

//水位有两个：低水位和高水位。
//低水位:
//就是当可读的数据量到达这个低水位后，才会调用用户设置的回调函数。
//比如用户想每次读取100字节，那么就可以把低水位设置为100。
//当可读数据的字节数小于100时，即使有数据都不会打扰用户(即不会调用用户设置的回调函数)。
//可读数据大于等于100字节后，才会调用用户的回调函数。
//高水位:
//和用户的回调函数没有关系。
//它的意义是把读事件的evbuffer的数据量限制在高水位之下。
//比如，用户认为读缓冲区不能太大(太大的话，链表会很长)。
//那么用户就会设置读事件的高水位。
//当读缓冲区的数据量达到这个高水位后，即使socket fd还有数据没有读，也不会读进这个读缓冲区里面。
//一句话说，就是控制evbuffer的大小。

//虽然控制了evbuffer的大小，但socket fd可能还有数据。
//有数据就会触发可读事件，但处理可读的时候，又会发现设置了高水位，不能读取数据evbuffer。
//socket fd的数据没有被读完，又触发……。这个貌似是一个死循环。
//实际上是不会出现这个死循环的，因为Libevent发现evbuffer的数据量到达高水位后，就会把可读事件给挂起来，让它不能再触发了。
//Libevent使用函数bufferevent_wm_suspend_read把监听读事件的event挂起来。下面看一下Libevent是怎么把一个event挂起来的。
void
bufferevent_suspend_read(struct bufferevent *bufev, bufferevent_suspend_flags what)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	BEV_LOCK(bufev);
	if (!bufev_private->read_suspended)//不能挂多次 
		bufev->be_ops->disable(bufev, EV_READ);//实际调用be_socket_disable函数
	bufev_private->read_suspended |= what;////因何而被挂起
	BEV_UNLOCK(bufev);
}

void
bufferevent_unsuspend_read(struct bufferevent *bufev, bufferevent_suspend_flags what)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	BEV_LOCK(bufev);
	bufev_private->read_suspended &= ~what;
	if (!bufev_private->read_suspended && (bufev->enabled & EV_READ))
		bufev->be_ops->enable(bufev, EV_READ);////重新把event插入到event_base中
	BEV_UNLOCK(bufev);
}

void
bufferevent_suspend_write(struct bufferevent *bufev, bufferevent_suspend_flags what)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	BEV_LOCK(bufev);
	if (!bufev_private->write_suspended)
		bufev->be_ops->disable(bufev, EV_WRITE);
	bufev_private->write_suspended |= what;
	BEV_UNLOCK(bufev);
}

void
bufferevent_unsuspend_write(struct bufferevent *bufev, bufferevent_suspend_flags what)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	BEV_LOCK(bufev);
	bufev_private->write_suspended &= ~what;
	if (!bufev_private->write_suspended && (bufev->enabled & EV_WRITE))
		bufev->be_ops->enable(bufev, EV_WRITE);
	BEV_UNLOCK(bufev);
}

//现在假设用户移除了一些evbuffer的数据，进而触发了evbuffer的回调函数，
//当然也就调用了函数bufferevent_inbuf_wm_cb。
//下面看一下这个函数是怎么恢复读的。

//因为用户可以手动为这个evbuffer添加数据，此时也会调用bufferevent_inbuf_wm_cb函数。
//此时就要检查evbuffer的数据量是否已经超过高水位了，而不能仅仅检查是否低于高水位。
//高水位导致读的挂起和之后读的恢复，一切工作都是由Libevent内部完成的，用户不用做任何工作。
static void
bufferevent_inbuf_wm_cb(struct evbuffer *buf,
    const struct evbuffer_cb_info *cbinfo,
    void *arg)
{
	struct bufferevent *bufev = arg;
	size_t size;

	size = evbuffer_get_length(buf);

	if (size >= bufev->wm_read.high)
		bufferevent_wm_suspend_read(bufev);
	else
		bufferevent_wm_unsuspend_read(bufev);
}

static void
bufferevent_run_deferred_callbacks_locked(struct deferred_cb *_, void *arg)
{
	struct bufferevent_private *bufev_private = arg;
	struct bufferevent *bufev = &bufev_private->bev;

	BEV_LOCK(bufev);
	if ((bufev_private->eventcb_pending & BEV_EVENT_CONNECTED) &&
	    bufev->errorcb) {
		/* The "connected" happened before any reads or writes, so
		   send it first. */
		bufev_private->eventcb_pending &= ~BEV_EVENT_CONNECTED;
		bufev->errorcb(bufev, BEV_EVENT_CONNECTED, bufev->cbarg);
	}
	if (bufev_private->readcb_pending && bufev->readcb) {
		bufev_private->readcb_pending = 0;
		bufev->readcb(bufev, bufev->cbarg);
	}
	if (bufev_private->writecb_pending && bufev->writecb) {
		bufev_private->writecb_pending = 0;
		bufev->writecb(bufev, bufev->cbarg);
	}
	if (bufev_private->eventcb_pending && bufev->errorcb) {
		short what = bufev_private->eventcb_pending;
		int err = bufev_private->errno_pending;
		bufev_private->eventcb_pending = 0;
		bufev_private->errno_pending = 0;
		EVUTIL_SET_SOCKET_ERROR(err);
		bufev->errorcb(bufev, what, bufev->cbarg);
	}
	_bufferevent_decref_and_unlock(bufev);
}//end bufferevent_run_deferred_callbacks_locked

static void
bufferevent_run_deferred_callbacks_unlocked(struct deferred_cb *_, void *arg)
{
	struct bufferevent_private *bufev_private = arg;
	struct bufferevent *bufev = &bufev_private->bev;

	BEV_LOCK(bufev);
#define UNLOCKED(stmt) \
	do { BEV_UNLOCK(bufev); stmt; BEV_LOCK(bufev); } while(0)

	if ((bufev_private->eventcb_pending & BEV_EVENT_CONNECTED) &&
	    bufev->errorcb) {
		/* The "connected" happened before any reads or writes, so
		   send it first. */
		bufferevent_event_cb errorcb = bufev->errorcb;
		void *cbarg = bufev->cbarg;
		bufev_private->eventcb_pending &= ~BEV_EVENT_CONNECTED;
		UNLOCKED(errorcb(bufev, BEV_EVENT_CONNECTED, cbarg));
	}
	if (bufev_private->readcb_pending && bufev->readcb) {
		bufferevent_data_cb readcb = bufev->readcb;
		void *cbarg = bufev->cbarg;
		bufev_private->readcb_pending = 0;
		UNLOCKED(readcb(bufev, cbarg));
	}
	if (bufev_private->writecb_pending && bufev->writecb) {
		bufferevent_data_cb writecb = bufev->writecb;
		void *cbarg = bufev->cbarg;
		bufev_private->writecb_pending = 0;
		UNLOCKED(writecb(bufev, cbarg));
	}
	if (bufev_private->eventcb_pending && bufev->errorcb) {
		bufferevent_event_cb errorcb = bufev->errorcb;
		void *cbarg = bufev->cbarg;
		short what = bufev_private->eventcb_pending;
		int err = bufev_private->errno_pending;
		bufev_private->eventcb_pending = 0;
		bufev_private->errno_pending = 0;
		EVUTIL_SET_SOCKET_ERROR(err);
		UNLOCKED(errorcb(bufev,what,cbarg));
	}
	_bufferevent_decref_and_unlock(bufev);
#undef UNLOCKED
}//end bufferevent_run_deferred_callbacks_unlocked


#define SCHEDULE_DEFERRED(bevp)						\
	do {								\
		bufferevent_incref(&(bevp)->bev);			\
		event_deferred_cb_schedule(				\
			event_base_get_deferred_cb_queue((bevp)->bev.ev_base), \
			&(bevp)->deferred);				\
	} while (0)

void
_bufferevent_run_readcb(struct bufferevent *bufev)
{
	/* Requires that we hold the lock and a reference */
	struct bufferevent_private *p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	if (bufev->readcb == NULL)
		return;
	if (p->options & BEV_OPT_DEFER_CALLBACKS) {
		p->readcb_pending = 1;
		if (!p->deferred.queued)
			SCHEDULE_DEFERRED(p);
	} else {
		bufev->readcb(bufev, bufev->cbarg);
	}
}

void
_bufferevent_run_writecb(struct bufferevent *bufev)
{
	/* Requires that we hold the lock and a reference */
	struct bufferevent_private *p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	if (bufev->writecb == NULL)
		return;
	if (p->options & BEV_OPT_DEFER_CALLBACKS) {
		p->writecb_pending = 1;
		if (!p->deferred.queued)
			SCHEDULE_DEFERRED(p);
	} else {
		bufev->writecb(bufev, bufev->cbarg);
	}
}

void
_bufferevent_run_eventcb(struct bufferevent *bufev, short what)
{
	/* Requires that we hold the lock and a reference */
	struct bufferevent_private *p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	if (bufev->errorcb == NULL)
		return;
	if (p->options & BEV_OPT_DEFER_CALLBACKS) {
		p->eventcb_pending |= what;
		p->errno_pending = EVUTIL_SOCKET_ERROR();
		if (!p->deferred.queued)
			SCHEDULE_DEFERRED(p);
	} else {
		bufev->errorcb(bufev, what, bufev->cbarg);
	}
}

//在bufferevent_socket_new函数里面会调用函数bufferevent_init_common完成公有部分的初始化。
int
bufferevent_init_common(struct bufferevent_private *bufev_private,
    struct event_base *base,
    const struct bufferevent_ops *ops,
    enum bufferevent_options options)
{
	struct bufferevent *bufev = &bufev_private->bev;

    //分配输入缓冲区  
	if (!bufev->input) {
		if ((bufev->input = evbuffer_new()) == NULL)
			return -1;
	}

    //分配输出缓冲区  
	if (!bufev->output) {
		if ((bufev->output = evbuffer_new()) == NULL) {
			evbuffer_free(bufev->input);
			return -1;
		}
	}

	//引用次数为1  
	bufev_private->refcnt = 1;
	bufev->ev_base = base;

	/* Disable timeouts. */
    //默认情况下,读和写event都是不支持超时的  
	evutil_timerclear(&bufev->timeout_read);
	evutil_timerclear(&bufev->timeout_write);

	bufev->be_ops = ops;

	/*
	 * Set to EV_WRITE so that using bufferevent_write is going to
	 * trigger a callback.  Reading needs to be explicitly enabled
	 * because otherwise no data will be available.
	 */
     //可写是默认支持的 
	bufev->enabled = EV_WRITE;

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	if (options & BEV_OPT_THREADSAFE) {
		//申请锁。
		if (bufferevent_enable_locking(bufev, NULL) < 0) {
			/* cleanup */
			evbuffer_free(bufev->input);
			evbuffer_free(bufev->output);
			bufev->input = NULL;
			bufev->output = NULL;
			return -1;
		}
	}
#endif
	//延迟调用的初始化，一般不需要用到
	if ((options & (BEV_OPT_DEFER_CALLBACKS|BEV_OPT_UNLOCK_CALLBACKS))
	    == BEV_OPT_UNLOCK_CALLBACKS) {
		event_warnx("UNLOCK_CALLBACKS requires DEFER_CALLBACKS");
		return -1;
	}
	if (options & BEV_OPT_DEFER_CALLBACKS) {
		if (options & BEV_OPT_UNLOCK_CALLBACKS)
			event_deferred_cb_init(&bufev_private->deferred,
			    bufferevent_run_deferred_callbacks_unlocked,
			    bufev_private);
		else
			event_deferred_cb_init(&bufev_private->deferred,
			    bufferevent_run_deferred_callbacks_locked,
			    bufev_private);
	}

	bufev_private->options = options;

    //将evbuffer和bufferevent相关联  
	evbuffer_set_parent(bufev->input, bufev);
	evbuffer_set_parent(bufev->output, bufev);

	return 0;
}//end bufferevent_init_common


void
bufferevent_setcb(struct bufferevent *bufev,
    bufferevent_data_cb readcb, bufferevent_data_cb writecb,
    bufferevent_event_cb eventcb, void *cbarg)
{
    //bufferevent结构体内部有一个锁变量  
	BEV_LOCK(bufev);
	//如果不想设置某个操作的回调函数，直接设置为NULL即可
	bufev->readcb = readcb;
	bufev->writecb = writecb;
	bufev->errorcb = eventcb;

	bufev->cbarg = cbarg;
	BEV_UNLOCK(bufev);
}

//现在来看一下，把监听写事件的event从event_base的插入队列中删除后，
//如果下次用户有数据要写的时候，怎么把这个event添加到event_base的插入队列。

//用户一般是通过bufferevent_write函数把数据写入到evbuffer
//(写入evbuffer后，接着就会被写入socket，所以调用bufferevent_write就相当于把数据写入到socket。)。
//而这个bufferevent_write函数是直接调用evbuffer_add函数的。
//函数evbuffer_add没有调用什么可疑的函数，能够把监听可写的event添加到event_base中。
//唯一的可能就是那个回调函数。对就是evbuffer的回调函数。关于evbuffer的回调函数，可以参考这里。
//evbuffer_add中的evbuffer_invoke_callbacks(buf);实现此功能
//这个buf实际就是bufferevent_socket_new中的 evbuffer_add_cb(bufev->output,bufferevent_socket_outbuf_cb, bufev); 
//当bufferevent的写缓冲区output的数据发生变化时，函数bufferevent_socket_outbuf_cb就会被调用。

//对于bufferevent_write，初次使用该函数的读者可能会有疑问：
//调用该函数后，参数data指向的内存空间能不能马上释放，还是要等到Libevent把data指向的数据都写到socket 缓存区才能删除？
//其实，从前一篇博文可以看到，evbuffer_add是直接复制一份用户要发送的数据到evbuffer缓存区的。
//所以，调用完bufferevent_write，就可以马上释放参数data指向的内存空间。

//从上面的分析可以得知在主线程中调用bufferevent_write函数写入数据的是可以的。
//是可以马上把监听可写事件的event添加到event_base中。

//如果是在次线程调用该函数写入数据呢？
//此时，主线程可能还睡眠在poll、epoll这类的多路IO复用函数上。
//这种情况下能不能及时唤醒主线程呢？
//其实是可以的，只要你的Libevent在一开始使用了线程功能。
//具体的分析过程可以参考《evthread_notify_base通知主线程》。
//上面代码中的be_socket_add会调用event_add，而在次线程调用event_add就会调用evthread_notify_base通知主线程。
int
bufferevent_write(struct bufferevent *bufev, const void *data, size_t size)
{
	if (evbuffer_add(bufev->output, data, size) == -1)
		return (-1);

	return 0;
}

size_t
bufferevent_read(struct bufferevent *bufev, void *data, size_t size)
{
	return (evbuffer_remove(bufev->input, data, size));
}

//令bufferevent可以工作:
//即使调用了bufferevent_socket_new和bufferevent_setcb，这个bufferevent还是不能工作，必须调用bufferevent_enable。为什么会这样的呢？
//如果看过之前的那些博文，相信读者知道，一个event能够工作，不仅仅需要new出来，还要调用event_add函数，把这个event添加到event_base中。
//在本文前面的代码中，并没有看到event_add函数的调用。所以还需要调用一个函数，把event添加到event_base中。函数bufferevent_enable就是完成这个工作的。
int
bufferevent_enable(struct bufferevent *bufev, short event)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	short impl_events = event;
	int r = 0;

    //增加引用并加锁  
    //增加引用是为了防止其他线程调用bufferevent_free，释放了bufferevent  
	_bufferevent_incref_and_lock(bufev);

	//挂起了读，此时不能监听读事件
	if (bufev_private->read_suspended)
		impl_events &= ~EV_READ;

	//挂起了写，此时不能监听写事情
	if (bufev_private->write_suspended)
		impl_events &= ~EV_WRITE;

	bufev->enabled |= event;
	//调用对应类型的enbale函数。因为不同类型的bufferevent有不同的enable函数
	if (impl_events && bufev->be_ops->enable(bufev, impl_events) < 0)
		r = -1;

	//减少引用并解锁 
	_bufferevent_decref_and_unlock(bufev);
	return r;
}

int
bufferevent_disable(struct bufferevent *bufev, short event)
{
	int r = 0;

	BEV_LOCK(bufev);
	bufev->enabled &= ~event;

	if (bufev->be_ops->disable(bufev, event) < 0)
		r = -1;

	BEV_UNLOCK(bufev);
	return r;
}

//bufferevent_setwatermark函数时，高水位的参数设为0即不设置高水位。

//那么什么时候取消挂起，让bufferevent可以继续读socket 数据呢？
//从高水位的意义来说，当然是当evbuffer里面的数据量小于高水位时，就能再次读取socket数据了。
//现在来看一下Libevent是怎么恢复读的。

//看一下设置水位的函数bufferevent_setwatermark吧，它进行了一些为高水位埋下了一个回调函数。
//对，就是evbuffer的回调函数。前一篇博文说到，当evbuffer里面的数据添加或者删除时，是会触发一些回调函数的。
//当用户移除evbuffer的一些数据量时，Libevent就会检查这个evbuffer的数据量是否小于高水位，
//如果小于的话，那么就恢复 读事件。

//这个函数:
//1.高水位设置回调函数
//2.还会检查当前evbuffer的数据量是否超过了高水位。
//因为这个设置水位函数可能是在bufferevent工作一段时间后才添加的，所以evbuffer是有可能已经有数据的了，
//2-1.
//因此需要检查。如果超过了水位值，那么就需要挂起读。
//2-2.
//当然也存在另外一种可能：用户之前设置过了一个比较大的高水位，挂起了读。
//现在发现错了，就把高水位调低一点，此时就需要恢复读。
void
bufferevent_setwatermark(struct bufferevent *bufev, short events,
    size_t lowmark, size_t highmark)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);

	BEV_LOCK(bufev);
	if (events & EV_WRITE) {
		bufev->wm_write.low = lowmark;
		bufev->wm_write.high = highmark;
	}

	if (events & EV_READ) {
		bufev->wm_read.low = lowmark;
		bufev->wm_read.high = highmark;

		if (highmark) {//高水位 
			/* There is now a new high-water mark for read.
			   enable the callback if needed, and see if we should
			   suspend/bufferevent_wm_unsuspend. */

            //还没设置高水位的回调函数  
			if (bufev_private->read_watermarks_cb == NULL) {
				bufev_private->read_watermarks_cb =
				    evbuffer_add_cb(bufev->input,
						    bufferevent_inbuf_wm_cb,
						    bufev);
			}
			evbuffer_cb_set_flags(bufev->input,
				      bufev_private->read_watermarks_cb,
				      EVBUFFER_CB_ENABLED|EVBUFFER_CB_NODEFER);

            //设置(修改)高水位时，evbuffer的数据量已经超过了水位值  
            //可能是把之前的高水位调高或者调低  
            //挂起操作和取消挂起操作都是幂等的(即多次挂起的作用等同于挂起一次)
			if (evbuffer_get_length(bufev->input) > highmark)
				bufferevent_wm_suspend_read(bufev);
			else if (evbuffer_get_length(bufev->input) < highmark)
				bufferevent_wm_unsuspend_read(bufev);
		} else {
            //高水位值等于0，那么就要取消挂起 读事件  
            //取消挂起操作是幂等的 
			/* There is now no high-water mark for read. */
			if (bufev_private->read_watermarks_cb)
				evbuffer_cb_clear_flags(bufev->input,
				    bufev_private->read_watermarks_cb,
				    EVBUFFER_CB_ENABLED);
			bufferevent_wm_unsuspend_read(bufev);
		}
	}
	BEV_UNLOCK(bufev);
}//end bufferevent_setwatermark


void
_bufferevent_incref_and_lock(struct bufferevent *bufev)
{
	struct bufferevent_private *bufev_private =
	    BEV_UPCAST(bufev);
	BEV_LOCK(bufev);
	++bufev_private->refcnt;
}

int
_bufferevent_decref_and_unlock(struct bufferevent *bufev)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	struct bufferevent *underlying;

	EVUTIL_ASSERT(bufev_private->refcnt > 0);

	if (--bufev_private->refcnt) {
		BEV_UNLOCK(bufev);
		return 0;
	}

	underlying = bufferevent_get_underlying(bufev);

	/* Clean up the shared info */
	if (bufev->be_ops->destruct)
		bufev->be_ops->destruct(bufev);

	/* XXX what happens if refcnt for these buffers is > 1?
	 * The buffers can share a lock with this bufferevent object,
	 * but the lock might be destroyed below. */
	/* evbuffer will free the callbacks */
	evbuffer_free(bufev->input);
	evbuffer_free(bufev->output);

	if (bufev_private->rate_limiting) {
		if (bufev_private->rate_limiting->group)
			bufferevent_remove_from_rate_limit_group_internal(bufev,0);
		if (event_initialized(&bufev_private->rate_limiting->refill_bucket_event))
			event_del(&bufev_private->rate_limiting->refill_bucket_event);
		event_debug_unassign(&bufev_private->rate_limiting->refill_bucket_event);
		mm_free(bufev_private->rate_limiting);
		bufev_private->rate_limiting = NULL;
	}

	event_debug_unassign(&bufev->ev_read);
	event_debug_unassign(&bufev->ev_write);

	BEV_UNLOCK(bufev);
	if (bufev_private->own_lock)
		EVTHREAD_FREE_LOCK(bufev_private->lock,
		    EVTHREAD_LOCKTYPE_RECURSIVE);

	/* Free the actual allocated memory. */
	mm_free(((char*)bufev) - bufev->be_ops->mem_offset);

	if (underlying)
		bufferevent_decref(underlying);

	return 1;
}// end _bufferevent_decref_and_unlock


int
bufferevent_decref(struct bufferevent *bufev)
{
	BEV_LOCK(bufev);
	return _bufferevent_decref_and_unlock(bufev);
}

void
bufferevent_free(struct bufferevent *bufev)
{
	BEV_LOCK(bufev);
	bufferevent_setcb(bufev, NULL, NULL, NULL, NULL);
	_bufferevent_cancel_all(bufev);
	_bufferevent_decref_and_unlock(bufev);
}

void
bufferevent_incref(struct bufferevent *bufev)
{
	struct bufferevent_private *bufev_private =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);

	BEV_LOCK(bufev);
	++bufev_private->refcnt;
	BEV_UNLOCK(bufev);
}

int
bufferevent_enable_locking(struct bufferevent *bufev, void *lock)
{
#ifdef _EVENT_DISABLE_THREAD_SUPPORT
	return -1;
#else
	struct bufferevent *underlying;

	if (BEV_UPCAST(bufev)->lock)
		return -1;
	underlying = bufferevent_get_underlying(bufev);

	if (!lock && underlying && BEV_UPCAST(underlying)->lock) {
		lock = BEV_UPCAST(underlying)->lock;
		BEV_UPCAST(bufev)->lock = lock;
		BEV_UPCAST(bufev)->own_lock = 0;
	} else if (!lock) {
		EVTHREAD_ALLOC_LOCK(lock, EVTHREAD_LOCKTYPE_RECURSIVE);
		if (!lock)
			return -1;
		BEV_UPCAST(bufev)->lock = lock;
		BEV_UPCAST(bufev)->own_lock = 1;
	} else {
		BEV_UPCAST(bufev)->lock = lock;
		BEV_UPCAST(bufev)->own_lock = 0;
	}
	evbuffer_enable_locking(bufev->input, lock);
	evbuffer_enable_locking(bufev->output, lock);

	if (underlying && !BEV_UPCAST(underlying)->lock)
		bufferevent_enable_locking(underlying, lock);

	return 0;
#endif
} //end bufferevent_enable_locking

int
bufferevent_setfd(struct bufferevent *bev, evutil_socket_t fd)
{
	union bufferevent_ctrl_data d;
	int res = -1;
	d.fd = fd;
	BEV_LOCK(bev);
	if (bev->be_ops->ctrl)
		res = bev->be_ops->ctrl(bev, BEV_CTRL_SET_FD, &d);
	BEV_UNLOCK(bev);
	return res;
}

evutil_socket_t
bufferevent_getfd(struct bufferevent *bev)
{
	union bufferevent_ctrl_data d;
	int res = -1;
	d.fd = -1;
	BEV_LOCK(bev);
	if (bev->be_ops->ctrl)
		res = bev->be_ops->ctrl(bev, BEV_CTRL_GET_FD, &d);
	BEV_UNLOCK(bev);
	return (res<0) ? -1 : d.fd;
}

static void
_bufferevent_cancel_all(struct bufferevent *bev)
{
	union bufferevent_ctrl_data d;
	memset(&d, 0, sizeof(d));
	BEV_LOCK(bev);
	if (bev->be_ops->ctrl)
		bev->be_ops->ctrl(bev, BEV_CTRL_CANCEL_ALL, &d);
	BEV_UNLOCK(bev);
}

struct bufferevent *
bufferevent_get_underlying(struct bufferevent *bev)
{
	union bufferevent_ctrl_data d;
	int res = -1;
	d.ptr = NULL;
	BEV_LOCK(bev);
	if (bev->be_ops->ctrl)
		res = bev->be_ops->ctrl(bev, BEV_CTRL_GET_UNDERLYING, &d);
	BEV_UNLOCK(bev);
	return (res<0) ? NULL : d.ptr;
}

int
_bufferevent_add_event(struct event *ev, const struct timeval *tv)
{
	if (tv->tv_sec == 0 && tv->tv_usec == 0)
		return event_add(ev, NULL);
	else
		return event_add(ev, tv);
}


