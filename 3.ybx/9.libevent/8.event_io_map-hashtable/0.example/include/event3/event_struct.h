#ifndef _EVENT3_EVENT_STRUCT_H_
#define _EVENT3_EVENT_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#ifdef _EVENT_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* For int types. */
#include <event3/util.h>

#define EVLIST_TIMEOUT	0x01
#define EVLIST_INSERTED	0x02
#define EVLIST_SIGNAL	0x04
#define EVLIST_ACTIVE	0x08
#define EVLIST_INTERNAL	0x10
#define EVLIST_INIT	0x80

/* EVLIST_X_ Private space: 0x1000-0xf000 */
#define EVLIST_ALL	(0xf000 | 0x9f)

/* Fix so that people don't have to run with <sys/queue.h> */
#ifndef TAILQ_ENTRY
#define _EVENT_DEFINED_TQENTRY
#define TAILQ_ENTRY(type)                       \
struct {                                \
    struct type *tqe_next;  /* next element */          \
    struct type **tqe_prev; /* address of previous next element */  \
}
#endif /* !TAILQ_ENTRY */

#ifndef TAILQ_HEAD
#define _EVENT_DEFINED_TQHEAD
#define TAILQ_HEAD(name, type)          \
struct name {                   \
    struct type *tqh_first;         \
    struct type **tqh_last;         \
}
#endif

struct event_base;
struct event {
    TAILQ_ENTRY(event) ev_active_next;
    TAILQ_ENTRY(event) ev_next;
	//hplsp page 255
    /* for managing timeouts */
    union {
		//用于通用定时器(尾队列)找到位置
        TAILQ_ENTRY(event) ev_next_with_common_timeout;
		//用于最小堆定时器找到定时器位置
        int min_heap_idx;
    } ev_timeout_pos;
    evutil_socket_t ev_fd;

    struct event_base *ev_base;

    union {
        /* used for io events */
        struct {
            TAILQ_ENTRY(event) ev_io_next;
            struct timeval ev_timeout;
        } ev_io;

        /* used by signal events */
        struct {
            TAILQ_ENTRY(event) ev_signal_next;
            short ev_ncalls;
            /* Allows deletes in callback */
            short *ev_pncalls;
        } ev_signal;
    } _ev;

    short ev_events;
    short ev_res;       /* result passed to event callback */
    short ev_flags;
    ev_uint8_t ev_pri;  /* smaller numbers are higher priority */
    ev_uint8_t ev_closure;
    struct timeval ev_timeout;

    /* allows us to adopt for different types of events */
    void (*ev_callback)(evutil_socket_t, short, void *arg);
    void *ev_arg;
};

TAILQ_HEAD (event_list, event);

#ifdef __cplusplus
}
#endif
#endif

