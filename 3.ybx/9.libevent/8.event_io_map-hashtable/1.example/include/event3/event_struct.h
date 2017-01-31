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

TAILQ_HEAD (event_list, event);

#ifdef __cplusplus
}
#endif
#endif

