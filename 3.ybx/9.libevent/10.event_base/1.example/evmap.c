#include <event3/event-config.h>

#include <sys/types.h>
#if defined(_EVENT_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include "event-internal.h"
#include "evmap-internal.h"
#include "mm-internal.h"
#include "changelist-internal.h"

struct evmap_io {
    struct event_list events;
    ev_uint16_t nread;
    ev_uint16_t nwrite;
};

//event_io_map 非hashtable结构
struct evmap_signal {  
    struct event_list events;  
};  

#ifdef EVMAP_USE_HT
struct event_map_entry {
    HT_ENTRY(event_map_entry) map_node;
    evutil_socket_t fd;
    union {
        struct evmap_io evmap_io;
    } ent;
};

static inline unsigned
hashsocket(struct event_map_entry *e)
{
    unsigned h = (unsigned) e->fd;
    h += (h >> 2) | (h << 30);
    return h;
}

static inline int
eqsocket(struct event_map_entry *e1, struct event_map_entry *e2)
{
    return e1->fd == e2->fd;
}

HT_PROTOTYPE(event_io_map, event_map_entry, map_node, hashsocket, eqsocket)
HT_GENERATE(event_io_map, event_map_entry, map_node, hashsocket, eqsocket,
            0.5, mm_malloc, mm_realloc, mm_free)

#define GET_IO_SLOT_AND_CTOR(x, map, slot, type, ctor, fdinfo_len)	\
	do {								\
		struct event_map_entry _key, *_ent;			\
		_key.fd = slot;						\
		_HT_FIND_OR_INSERT(event_io_map, map_node, hashsocket, map, \
		    event_map_entry, &_key, ptr,			\
		    {							\
			    _ent = *ptr;				\
		    },							\
		    {							\
			    _ent = mm_calloc(1,sizeof(struct event_map_entry)+fdinfo_len); \
			    if (EVUTIL_UNLIKELY(_ent == NULL))		\
				    return (-1);			\
			    _ent->fd = slot;				\
			    (ctor)(&_ent->ent.type);			\
			    _HT_FOI_INSERT(map_node, map, &_key, _ent, ptr) \
				});					\
		(x) = &_ent->ent.type;					\
	} while (0)


void evmap_io_initmap(struct event_io_map *ctx)
{
    HT_INIT(event_io_map, ctx);
}

void evmap_io_clear(struct event_io_map *ctx)
{
    struct event_map_entry **ent, **next, *this;
    for (ent = HT_START(event_io_map, ctx); ent; ent = next) {
        this = *ent;
        next = HT_NEXT_RMV(event_io_map, ctx, ent);
        mm_free(this);
    }
    HT_CLEAR(event_io_map, ctx); /* remove all storage held by the ctx. */
}

#endif //ifdef EVMAP_USE_HT


#define GET_SIGNAL_SLOT(x, map, slot, type)			\
	(x) = (struct type *)((map)->entries[slot])

//GET_SIGNAL_SLOT_AND_CTOR宏的作用就是让ctx指向structevmap_signal结构体中的TAILQ_HEAD。
//这样就可以使用TAILQ_INSERT_TAIL宏，把ev变量插入到队列中。
//如果有现成的struct evmap_signal就直接使用，没有的话就新建一个。
//
//在非Windows系统中，event_io_map也被定义成了event_signal_map，但实际上他们并不会由链表连在一起。
//因为在event_base结构体中，分别有struct event_io_map变量io和event_signal_map变量sigmap。
//所有的io类型的event结构体会被放到io中，而信号类型的event则会被放到sigmap变量中。
#define GET_SIGNAL_SLOT_AND_CTOR(x, map, slot, type, ctor, fdinfo_len)	\
	do {								\
		if ((map)->entries[slot] == NULL) {			\
			(map)->entries[slot] =				\
			    mm_calloc(1,sizeof(struct type)+fdinfo_len); \
			if (EVUTIL_UNLIKELY((map)->entries[slot] == NULL)) \
				return (-1);				\
			(ctor)((struct type *)(map)->entries[slot]);	\
		}							\
		(x) = (struct type *)((map)->entries[slot]);		\
	} while (0)




#ifndef EVMAP_USE_HT
#define GET_IO_SLOT(x,map,slot,type) GET_SIGNAL_SLOT(x,map,slot,type)
#define GET_IO_SLOT_AND_CTOR(x,map,slot,type,ctor,fdinfo_len)	\
		GET_SIGNAL_SLOT_AND_CTOR(x,map,slot,type,ctor,fdinfo_len)

void
evmap_io_initmap(struct event_io_map* ctx)
{
	evmap_signal_initmap(ctx);
}

void
evmap_io_clear(struct event_io_map* ctx)
{
	evmap_signal_clear(ctx);
}
#endif //not define EVMAP_USE_HT


//slot是信号值sig，或者文件描述符fd.  
//当sig或者fd >= map的nentries变量时就会调用此函数  
static int
evmap_make_space(struct event_signal_map *map, int slot, int msize)
{
	if (map->nentries <= slot) {
        //posix标准中，信号的种类就只有32种。  
        //在Windows中，信号的种类就更少了，只有6种。  
        //所以一开始取32还是比较合理的  
		int nentries = map->nentries ? map->nentries : 32;
		void **tmp;

        //当slot是一个文件描述符时，就会大于32  
		while (nentries <= slot)
			nentries <<= 1;

		tmp = (void **)mm_realloc(map->entries, nentries * msize);
		if (tmp == NULL)
			return (-1);

        //清零是很有必要的。因为tmp是二级指针，数组里面的元素是一个指针  
		memset(&tmp[map->nentries], 0,
		    (nentries - map->nentries) * msize);

		map->nentries = nentries;
		map->entries = tmp;
	}

	return (0);
}

void
evmap_signal_initmap(struct event_signal_map *ctx)
{
	ctx->nentries = 0;
	ctx->entries = NULL;
}

void
evmap_signal_clear(struct event_signal_map *ctx)
{
	if (ctx->entries != NULL) {
		int i;
		for (i = 0; i < ctx->nentries; ++i) {
			if (ctx->entries[i] != NULL)
				mm_free(ctx->entries[i]);
		}
		mm_free(ctx->entries);
		ctx->entries = NULL;
	}
	ctx->nentries = 0;
}

static void
evmap_io_init(struct evmap_io *entry)
{
	TAILQ_INIT(&entry->events);
	entry->nread = 0;
	entry->nwrite = 0;
}

/* return -1 on error, 0 on success if nothing changed in the event backend,
 * and 1 on success if something did. */
int
evmap_io_add(struct event_base *base, evutil_socket_t fd, struct event *ev)
{
	const struct eventop *evsel = base->evsel;
	struct event_io_map *io = &base->io;
	struct evmap_io *ctx = NULL;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;
	struct event *old_ev;

	EVUTIL_ASSERT(fd == ev->ev_fd);

	if (fd < 0)
		return 0;

#ifndef EVMAP_USE_HT
	if (fd >= io->nentries) {
		if (evmap_make_space(io, fd, sizeof(struct evmap_io *)) == -1)
			return (-1);
	}
#endif
	GET_IO_SLOT_AND_CTOR(ctx, io, fd, evmap_io, evmap_io_init,
						 evsel->fdinfo_len);

	nread = ctx->nread;
	nwrite = ctx->nwrite;

	if (nread)
		old |= EV_READ;
	if (nwrite)
		old |= EV_WRITE;

	if (ev->ev_events & EV_READ) {
		if (++nread == 1)
			res |= EV_READ;
	}
	if (ev->ev_events & EV_WRITE) {
		if (++nwrite == 1)
			res |= EV_WRITE;
	}
	if (EVUTIL_UNLIKELY(nread > 0xffff || nwrite > 0xffff)) {
		event_warnx("Too many events reading or writing on fd %d",
		    (int)fd);
		return -1;
	}
	if (EVENT_DEBUG_MODE_IS_ON() &&
	    (old_ev = TAILQ_FIRST(&ctx->events)) &&
	    (old_ev->ev_events&EV_ET) != (ev->ev_events&EV_ET)) {
		event_warnx("Tried to mix edge-triggered and non-edge-triggered"
		    " events on fd %d", (int)fd);
		return -1;
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof(struct evmap_io);
		/* XXX(niels): we cannot mix edge-triggered and
		 * level-triggered, we should probably assert on
		 * this. */
		if (evsel->add(base, ev->ev_fd,
			old, (ev->ev_events & EV_ET) | res, extra) == -1)
			return (-1);
		retval = 1;
	}

	ctx->nread = (ev_uint16_t) nread;
	ctx->nwrite = (ev_uint16_t) nwrite;
	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_io_next);

	return (retval);
} // end evmap_io_add

/* return -1 on error, 0 on success if nothing changed in the event backend,
 * and 1 on success if something did. */
int
evmap_io_del(struct event_base *base, evutil_socket_t fd, struct event *ev)
{
	const struct eventop *evsel = base->evsel;
	struct event_io_map *io = &base->io;
	struct evmap_io *ctx;
	int nread, nwrite, retval = 0;
	short res = 0, old = 0;

	if (fd < 0)
		return 0;

	EVUTIL_ASSERT(fd == ev->ev_fd);

#ifndef EVMAP_USE_HT
	if (fd >= io->nentries)
		return (-1);
#endif

	GET_IO_SLOT(ctx, io, fd, evmap_io);

	nread = ctx->nread;
	nwrite = ctx->nwrite;

	if (nread)
		old |= EV_READ;
	if (nwrite)
		old |= EV_WRITE;

	if (ev->ev_events & EV_READ) {
		if (--nread == 0)
			res |= EV_READ;
		EVUTIL_ASSERT(nread >= 0);
	}
	if (ev->ev_events & EV_WRITE) {
		if (--nwrite == 0)
			res |= EV_WRITE;
		EVUTIL_ASSERT(nwrite >= 0);
	}

	if (res) {
		void *extra = ((char*)ctx) + sizeof(struct evmap_io);
		if (evsel->del(base, ev->ev_fd, old, res, extra) == -1)
			return (-1);
		retval = 1;
	}

	ctx->nread = nread;
	ctx->nwrite = nwrite;
	TAILQ_REMOVE(&ctx->events, ev, ev_io_next);

	return (retval);
}

void
evmap_io_active(struct event_base *base, evutil_socket_t fd, short events)
{
	struct event_io_map *io = &base->io;
	struct evmap_io *ctx;
	struct event *ev;

#ifndef EVMAP_USE_HT
	EVUTIL_ASSERT(fd < io->nentries);
#endif
	GET_IO_SLOT(ctx, io, fd, evmap_io);

	EVUTIL_ASSERT(ctx);
	TAILQ_FOREACH(ev, &ctx->events, ev_io_next) {
		if (ev->ev_events & events)
			event_active_nolock(ev, ev->ev_events & events, 1);
	}
}

static void
evmap_signal_init(struct evmap_signal *entry)
{
	TAILQ_INIT(&entry->events);
}

int
evmap_signal_add(struct event_base *base, int sig, struct event *ev)
{
	const struct eventop *evsel = base->evsigsel;
	struct event_signal_map *map = &base->sigmap;
	struct evmap_signal *ctx = NULL;

	if (sig >= map->nentries) {
		if (evmap_make_space(
			map, sig, sizeof(struct evmap_signal *)) == -1)
			return (-1);
	}

	//do  
    //{
    //    //同event_io_map一样，同一个信号或者fd可以被多次event_new、event_add  
    //    //所以，当同一个信号或者fd被多次event_add后，entries[sig]就不会为NULL  
    //    if ((map)->entries[sig] == NULL)//第一次  
    //    {  
    //        //evmap_signal成员只有一个TAILQ_HEAD (event_list, event);  
    //        //可以说evmap_signal本身就是一个TAILQ_HEAD  
    //        //这个赋值操作很重要。  
    //        (map)->entries[sig] = mm_calloc(1, sizeof(struct evmap_signal)  
    //                                           + base->evsigsel->fdinfo_len  
    //                                       );  
    //        if (EVUTIL_UNLIKELY((map)->entries[sig] == NULL))  
    //            return (-1);  
    //        //内部调用TAILQ_INIT(&entry->events);  
    //        (evmap_signal_init)((struct evmap_signal *)(map)->entries[sig]);  
    //    }  
    //    (ctx) = (struct evmap_signal *)((map)->entries[sig]);  
    //} while (0);  
	GET_SIGNAL_SLOT_AND_CTOR(ctx, map, sig, evmap_signal, evmap_signal_init,
	    base->evsigsel->fdinfo_len);

	if (TAILQ_EMPTY(&ctx->events)) {
		if (evsel->add(base, ev->ev_fd, 0, EV_SIGNAL, NULL)
		    == -1)
			return (-1);
	}

	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_signal_next);

	return (1);
} //end of evmap_signal_add


int
evmap_signal_del(struct event_base *base, int sig, struct event *ev)
{
	const struct eventop *evsel = base->evsigsel;
	struct event_signal_map *map = &base->sigmap;
	struct evmap_signal *ctx;

	if (sig >= map->nentries)
		return (-1);

	GET_SIGNAL_SLOT(ctx, map, sig, evmap_signal);

	if (TAILQ_FIRST(&ctx->events) == TAILQ_LAST(&ctx->events, event_list)) {
		if (evsel->del(base, ev->ev_fd, 0, EV_SIGNAL, NULL) == -1)
			return (-1);
	}

	TAILQ_REMOVE(&ctx->events, ev, ev_signal_next);

	return (1);
}

void
evmap_signal_active(struct event_base *base, evutil_socket_t sig, int ncalls)
{
	struct event_signal_map *map = &base->sigmap;
	struct evmap_signal *ctx;
	struct event *ev;

	EVUTIL_ASSERT(sig < map->nentries);
	GET_SIGNAL_SLOT(ctx, map, sig, evmap_signal);

	TAILQ_FOREACH(ev, &ctx->events, ev_signal_next)
		event_active_nolock(ev, EV_SIGNAL, ncalls);
}


void
event_changelist_init(struct event_changelist *changelist)
{
	changelist->changes = NULL;
	changelist->changes_size = 0;
	changelist->n_changes = 0;
}

void
event_changelist_freemem(struct event_changelist *changelist)
{
	if (changelist->changes)
		mm_free(changelist->changes);
	event_changelist_init(changelist); /* zero it all out. */
}
