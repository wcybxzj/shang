#include "event2/event-config.h"

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
