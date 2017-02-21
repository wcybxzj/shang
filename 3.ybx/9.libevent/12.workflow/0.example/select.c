#include <event3/event-config.h>

#include <sys/types.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef _EVENT_HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/queue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "event-internal.h"
#include "evsignal-internal.h"
#include <event3/thread.h>
#include "evthread-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"

#ifndef _EVENT_HAVE_FD_MASK
/* This type is mandatory, but Android doesn't define it. */
typedef unsigned long fd_mask;
#endif

#ifndef NFDBITS
#define NFDBITS (sizeof(fd_mask)*8)
#endif

/* Divide positive x by y, rounding up. */
#define DIV_ROUNDUP(x, y)   (((x)+((y)-1))/(y))

/* How many bytes to allocate for N fds? */
#define SELECT_ALLOC_SIZE(n) \
	(DIV_ROUNDUP(n, NFDBITS) * sizeof(fd_mask))


struct selectop {
	// 在集合中最大的文件描述符的数值
	int event_fds;		/* Highest fd in fd set */
	// 集合的大小
	int event_fdsz;
	///* 标志位，在select_resize函数中对event_readset_in和event_writeset_in两个描述符集调整了存储空间后将该标志位置为1，
	//在select_dispatch函数中对event_readset_out和event_writeset_out两个描述符集调整了存储空间后将该标志位置为0 */  
	int resize_out_sets;
	// 读集合，但是在调用select之前需要将其复制到event_readset_out中
	fd_set *event_readset_in;
	// 写集合
	fd_set *event_writeset_in;
	// 读集合，作为select的输入（select函数回修改这些集合，所以在调用之前需要备份好）
	fd_set *event_readset_out;
	// 写集合，作为select的输入（select函数回修改这些集合，所以在调用之前需要备份好）
	fd_set *event_writeset_out;
};


static void *select_init(struct event_base *);
static int select_add(struct event_base *, int, short old, short events, void*);
static int select_del(struct event_base *, int, short old, short events, void*);
static int select_dispatch(struct event_base *, struct timeval *);
static void select_dealloc(struct event_base *);

const struct eventop selectops = {
	"select",
	select_init,
	select_add,
	select_del,
	select_dispatch,
	select_dealloc,
	0, /* doesn't need reinit. */
	EV_FEATURE_FDS,
	0,
};

static int select_resize(struct selectop *sop, int fdsz);
static void select_free_selectop(struct selectop *sop);


static void *
select_init(struct event_base *base)
{
	struct selectop *sop;

	if (!(sop = mm_calloc(1, sizeof(struct selectop))))
		return (NULL);

	// 分配select对象的套接字集合
	if (select_resize(sop, SELECT_ALLOC_SIZE(32 + 1))) {
		select_free_selectop(sop);
		return (NULL);
	}

	//printf("select init:%d\n", SELECT_ALLOC_SIZE(32+1));//8

	// 信号事件处理器初始化
	evsig_init(base);

	return (sop);
}

#ifdef CHECK_INVARIANTS
static void
check_selectop(struct selectop *sop)
{
	/* nothing to be done here */
}
#else
#define check_selectop(sop) do { (void) sop; } while (0)
#endif


static int
select_dispatch(struct event_base *base, struct timeval *tv)
{
	int res=0, i, j, nfds;
	struct selectop *sop = base->evbase;

	check_selectop(sop);
	//判断是否需要重新调整输出集合的大小
	//（如果event_readset_in的大小大于event_readset_out，
	//那么必须调整event_readset_out的大小）
	if (sop->resize_out_sets) {
		fd_set *readset_out=NULL, *writeset_out=NULL;
		size_t sz = sop->event_fdsz;
		if (!(readset_out = mm_realloc(sop->event_readset_out, sz)))
			return (-1);
		sop->event_readset_out = readset_out;
		if (!(writeset_out = mm_realloc(sop->event_writeset_out, sz))) {
			/* We don't free readset_out here, since it was
			 * already successfully reallocated. The next time
			 * we call select_dispatch, the realloc will be a
			 * no-op. */
			return (-1);
		}
		sop->event_writeset_out = writeset_out;
		sop->resize_out_sets = 0;
	}

	// 将备份集合中的数据复制到将要被select处理的集合中
	// 这样被修改的始终都是event_readset_out和event_writeset_out
	// 而event_readset_in和event_writeset_in中总是存放这最新的文件描述符集合
	memcpy(sop->event_readset_out, sop->event_readset_in,
	       sop->event_fdsz);
	memcpy(sop->event_writeset_out, sop->event_writeset_in,
	       sop->event_fdsz);

	nfds = sop->event_fds+1;

	EVBASE_RELEASE_LOCK(base, th_base_lock);

	res = select(nfds, sop->event_readset_out,
	    sop->event_writeset_out, NULL, tv);

	EVBASE_ACQUIRE_LOCK(base, th_base_lock);

	check_selectop(sop);

	if (res == -1) {
		if (errno != EINTR) {
			event_warn("select");
			return (-1);
		}

		return (0);
	}

	event_debug(("%s: select reports %d", __func__, res));

	check_selectop(sop);
  	//得到了select返回的结果后，首先我们知道select的麻烦在于其返回后的O(N)复杂度，
	//这里从N中随机取一个数开始循环，而不是每次都从0开始  
	i = random() % nfds;
	for (j = 0; j < nfds; ++j) {
		if (++i >= nfds)
			i = 0;
		res = 0;
		if (FD_ISSET(i, sop->event_readset_out))
			res |= EV_READ;
		if (FD_ISSET(i, sop->event_writeset_out))
			res |= EV_WRITE;

		if (res == 0)
			continue;
		// 将事件插入到已激活事件队列中，以进行下一步的操作
		evmap_io_active(base, i, res);
	}
	check_selectop(sop);

	return (0);
}//end select dispatch


static int
select_resize(struct selectop *sop, int fdsz)
{
	fd_set *readset_in = NULL;
	fd_set *writeset_in = NULL;

	if (sop->event_readset_in)
		check_selectop(sop);

	if ((readset_in = mm_realloc(sop->event_readset_in, fdsz)) == NULL)
		goto error;
	sop->event_readset_in = readset_in;
	if ((writeset_in = mm_realloc(sop->event_writeset_in, fdsz)) == NULL) {
		/* Note that this will leave event_readset_in expanded.
		 * That's okay; we wouldn't want to free it, since that would
		 * change the semantics of select_resize from "expand the
		 * readset_in and writeset_in, or return -1" to "expand the
		 * *set_in members, or trash them and return -1."
		 */
		goto error;
	}
	sop->event_writeset_in = writeset_in;
	sop->resize_out_sets = 1;

	memset((char *)sop->event_readset_in + sop->event_fdsz, 0,
	    fdsz - sop->event_fdsz);
	memset((char *)sop->event_writeset_in + sop->event_fdsz, 0,
	    fdsz - sop->event_fdsz);

	sop->event_fdsz = fdsz;
	check_selectop(sop);

	return (0);

 error:
	event_warn("malloc");
	return (-1);
}// end select_resize



static int
select_add(struct event_base *base, int fd, short old, short events, void *p)
{
	struct selectop *sop = base->evbase;
	(void) p;

	EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
	check_selectop(sop);
	/*
	 * Keep track of the highest fd, so that we can calculate the size
	 * of the fd_sets for select(2)
	 */
	// 如果当前记录的最大文件描述符小于fd，那么需要替换为fd
	// 即event_fds始终记录着集合中数值最大的套接字
	// 同时需要调整套接字集合大小
	if (sop->event_fds < fd) {
		int fdsz = sop->event_fdsz;

		if (fdsz < (int)sizeof(fd_mask))
			fdsz = (int)sizeof(fd_mask);

		/* In theory we should worry about overflow here.  In
		 * reality, though, the highest fd on a unixy system will
		 * not overflow here. XXXX */
		while (fdsz < (int) SELECT_ALLOC_SIZE(fd + 1))
			fdsz *= 2;

		if (fdsz != sop->event_fdsz) {
			if (select_resize(sop, fdsz)) {
				check_selectop(sop);
				return (-1);
			}
		}

		sop->event_fds = fd;
	}

	if (events & EV_READ)
		FD_SET(fd, sop->event_readset_in);
	if (events & EV_WRITE)
		FD_SET(fd, sop->event_writeset_in);
	check_selectop(sop);

	return (0);
}// end select_add


/*
 * Nothing to be done here.
 */
static int
select_del(struct event_base *base, int fd, short old, short events, void *p)
{
	struct selectop *sop = base->evbase;
	(void)p;

	EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
	check_selectop(sop);

	if (sop->event_fds < fd) {
		check_selectop(sop);
		return (0);
	}

	if (events & EV_READ)
		FD_CLR(fd, sop->event_readset_in);

	if (events & EV_WRITE)
		FD_CLR(fd, sop->event_writeset_in);

	check_selectop(sop);
	return (0);
}


static void
select_free_selectop(struct selectop *sop)
{
	if (sop->event_readset_in)
		mm_free(sop->event_readset_in);
	if (sop->event_writeset_in)
		mm_free(sop->event_writeset_in);
	if (sop->event_readset_out)
		mm_free(sop->event_readset_out);
	if (sop->event_writeset_out)
		mm_free(sop->event_writeset_out);

	memset(sop, 0, sizeof(struct selectop));
	mm_free(sop);
}

static void
select_dealloc(struct event_base *base)
{
	evsig_dealloc(base);

	select_free_selectop(base->evbase);
}

