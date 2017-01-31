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

#endif //EVMAP_USE_HT

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
}

