#include <event3/event-config.h>

#include <sys/types.h>
#if defined(_EVENT_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <sys/queue.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _EVENT_HAVE_SYS_EVENTFD_H
#include <sys/eventfd.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include <event3/event.h>
#include "event-internal.h"
#include "evthread-internal.h"
#include <event3/thread.h>
#include <event3/util.h>
#include "log-internal.h"
#include "evmap-internal.h"

#include "iocp-internal.h"
#include "changelist-internal.h"
#include "ht-internal.h"

#include "util-internal.h"

#ifdef _EVENT_HAVE_EVENT_PORTS
extern const struct eventop evportops;
#endif
#ifdef _EVENT_HAVE_SELECT
extern const struct eventop selectops;
#endif
#ifdef _EVENT_HAVE_POLL
//extern const struct eventop pollops;
#endif
#ifdef _EVENT_HAVE_EPOLL
//extern const struct eventop epollops;
#endif
#ifdef _EVENT_HAVE_WORKING_KQUEUE
extern const struct eventop kqops;
#endif
#ifdef _EVENT_HAVE_DEVPOLL
extern const struct eventop devpollops;
#endif
#ifdef WIN32
//extern const struct eventop win32ops;
#endif

/* Array of backends in order of preference. */
static const struct eventop *eventops[] = {
#ifdef _EVENT_HAVE_EVENT_PORTS
	&evportops,
#endif
#ifdef _EVENT_HAVE_WORKING_KQUEUE
	&kqops,
#endif
#ifdef _EVENT_HAVE_EPOLL
	//&epollops,
#endif
#ifdef _EVENT_HAVE_DEVPOLL
	&devpollops,
#endif
#ifdef _EVENT_HAVE_POLL
	//&pollops,
#endif
#ifdef _EVENT_HAVE_SELECT
	&selectops,
#endif
#ifdef WIN32
	//&win32ops,
#endif
	NULL
};

/* Global state; deprecated */
struct event_base *event_global_current_base_ = NULL;
#define current_base event_global_current_base_

/* Global state */
static int use_monotonic;

/* Prototypes */
static inline int event_add_internal(struct event *ev,
    const struct timeval *tv, int tv_is_absolute);
static inline int event_del_internal(struct event *ev);
static void	event_queue_insert(struct event_base *, struct event *, int);
static void	event_queue_remove(struct event_base *, struct event *, int);
static int	evthread_notify_base(struct event_base *base);

#ifndef _EVENT_DISABLE_DEBUG_MODE                                                                                              
struct event_debug_entry {
	HT_ENTRY(event_debug_entry) node;
	const struct event *ptr;
	unsigned added : 1;
};

static inline unsigned
hash_debug_entry(const struct event_debug_entry *e)
{
	/* We need to do this silliness to convince compilers that we
	 * honestly mean to cast e->ptr to an integer, and discard any
	 * part of it that doesn't fit in an unsigned.
	 */
	unsigned u = (unsigned) ((ev_uintptr_t) e->ptr);
	/* Our hashtable implementation is pretty sensitive to low bits,
	 * and every struct event is over 64 bytes in size, so we can
	 * just say >>6. */
	return (u >> 6);
}

static inline int
eq_debug_entry(const struct event_debug_entry *a,
    const struct event_debug_entry *b)
{
	return a->ptr == b->ptr;
}

int _event_debug_mode_on = 0;

/* Set if it's too late to enable event_debug_mode. */
static int event_debug_mode_too_late = 0;
#ifndef _EVENT_DISABLE_THREAD_SUPPORT                                                                                          
static void *_event_debug_map_lock = NULL;
#endif //end of _EVENT_DISABLE_THREAD_SUPPORT

static HT_HEAD(event_debug_map, event_debug_entry) global_debug_map =
	HT_INITIALIZER();

HT_PROTOTYPE(event_debug_map, event_debug_entry, node, hash_debug_entry,
		    eq_debug_entry)
HT_GENERATE(event_debug_map, event_debug_entry, node, hash_debug_entry,
			    eq_debug_entry, 0.5, mm_malloc, mm_realloc, mm_free)

/* Macro: record that ev is now setup (that is, ready for an add) */
#define _event_debug_note_setup(ev) do {				\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_FIND(event_debug_map, &global_debug_map, &find); \
		if (dent) {						\
			dent->added = 0;				\
		} else {						\
			dent = mm_malloc(sizeof(*dent));		\
			if (!dent)					\
				event_err(1,				\
				    "Out of memory in debugging code");	\
			dent->ptr = (ev);				\
			dent->added = 0;				\
			HT_INSERT(event_debug_map, &global_debug_map, dent); \
		}							\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	event_debug_mode_too_late = 1;					\
	} while (0)

/* Macro: record that ev is no longer setup */
#define _event_debug_note_teardown(ev) do {				\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_REMOVE(event_debug_map, &global_debug_map, &find); \
		if (dent)						\
			mm_free(dent);					\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	event_debug_mode_too_late = 1;					\
	} while (0)

/* Macro: record that ev is now added */
#define _event_debug_note_add(ev)	do {				\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_FIND(event_debug_map, &global_debug_map, &find); \
		if (dent) {						\
			dent->added = 1;				\
		} else {						\
			event_errx(_EVENT_ERR_ABORT,			\
			    "%s: noting an add on a non-setup event %p" \
			    " (events: 0x%x, fd: %d, flags: 0x%x)",	\
			    __func__, (ev), (ev)->ev_events,		\
			    (ev)->ev_fd, (ev)->ev_flags);		\
		}							\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	event_debug_mode_too_late = 1;					\
	} while (0)

/* Macro: record that ev is no longer added */
#define _event_debug_note_del(ev) do {					\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_FIND(event_debug_map, &global_debug_map, &find); \
		if (dent) {						\
			dent->added = 0;				\
		} else {						\
			event_errx(_EVENT_ERR_ABORT,			\
			    "%s: noting a del on a non-setup event %p"	\
			    " (events: 0x%x, fd: %d, flags: 0x%x)",	\
			    __func__, (ev), (ev)->ev_events,		\
			    (ev)->ev_fd, (ev)->ev_flags);		\
		}							\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	event_debug_mode_too_late = 1;					\
	} while (0)

/* Macro: assert that ev is setup (i.e., okay to add or inspect) */
#define _event_debug_assert_is_setup(ev) do {				\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_FIND(event_debug_map, &global_debug_map, &find); \
		if (!dent) {						\
			event_errx(_EVENT_ERR_ABORT,			\
			    "%s called on a non-initialized event %p"	\
			    " (events: 0x%x, fd: %d, flags: 0x%x)",	\
			    __func__, (ev), (ev)->ev_events,		\
			    (ev)->ev_fd, (ev)->ev_flags);		\
		}							\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	} while (0)


#define _event_debug_assert_not_added(ev) do {				\
	if (_event_debug_mode_on) {					\
		struct event_debug_entry *dent,find;			\
		find.ptr = (ev);					\
		EVLOCK_LOCK(_event_debug_map_lock, 0);			\
		dent = HT_FIND(event_debug_map, &global_debug_map, &find); \
		if (dent && dent->added) {				\
			event_errx(_EVENT_ERR_ABORT,			\
			    "%s called on an already added event %p"	\
			    " (events: 0x%x, fd: %d, flags: 0x%x)",	\
			    __func__, (ev), (ev)->ev_events,		\
			    (ev)->ev_fd, (ev)->ev_flags);		\
		}							\
		EVLOCK_UNLOCK(_event_debug_map_lock, 0);		\
	}								\
	} while (0)

#else //_EVENT_DISABLE_DEBUG_MODE                                                                                              

#define _event_debug_note_setup(ev) \
	((void)0)
#define _event_debug_note_teardown(ev) \
	((void)0)
#define _event_debug_note_add(ev) \
	((void)0)
#define _event_debug_note_del(ev) \
	((void)0)
#define _event_debug_assert_is_setup(ev) \
	((void)0)
#define _event_debug_assert_not_added(ev) \
	((void)0)
#endif //end of _EVENT_DISABLE_DEBUG_MODE

#define EVENT_BASE_ASSERT_LOCKED(base)		\
	EVLOCK_ASSERT_LOCKED((base)->th_base_lock)

/* The first time this function is called, it sets use_monotonic to 1
 * if we have a clock function that supports monotonic time */
static void
detect_monotonic(void)
{
#if defined(_EVENT_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	struct timespec	ts;
	static int use_monotonic_initialized = 0;

	if (use_monotonic_initialized)
		return;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
		use_monotonic = 1;

	use_monotonic_initialized = 1;
#endif
}


#define CLOCK_SYNC_INTERVAL -1

/** Set 'tp' to the current time according to 'base'.  We must hold the lock
 * on 'base'.  If there is a cached time, return it.  Otherwise, use
 * clock_gettime or gettimeofday as appropriate to find out the right time.
 * Return 0 on success, -1 on failure.
 */
static int
gettime(struct event_base *base, struct timeval *tp)
{
	EVENT_BASE_ASSERT_LOCKED(base);

	if (base->tv_cache.tv_sec) {
		*tp = base->tv_cache;
		return (0);
	}

#if defined(_EVENT_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		if (base->last_updated_clock_diff + CLOCK_SYNC_INTERVAL
		    < ts.tv_sec) {
			struct timeval tv;
			evutil_gettimeofday(&tv,NULL);
			evutil_timersub(&tv, tp, &base->tv_clock_diff);
			base->last_updated_clock_diff = ts.tv_sec;
		}

		return (0);
	}
#endif

	return (evutil_gettimeofday(tp, NULL));
}

struct event_base *
event_base_new(void)
{
	struct event_base *base = NULL;
	struct event_config *cfg = event_config_new();
	if (cfg) {
		base = event_base_new_with_config(cfg);
		event_config_free(cfg);
	}
	return base;
}

void
event_deferred_cb_queue_init(struct deferred_cb_queue *cb)
{
	memset(cb, 0, sizeof(struct deferred_cb_queue));
	TAILQ_INIT(&cb->deferred_cb_list);
}

/** Return true iff 'method' is the name of a method that 'cfg' tells us to
 * avoid. */
static int
event_config_is_avoided_method(const struct event_config *cfg,
    const char *method)
{
	struct event_config_entry *entry;

	TAILQ_FOREACH(entry, &cfg->entries, next) {
		if (entry->avoid_method != NULL &&
		    strcmp(entry->avoid_method, method) == 0)
			return (1);
	}

	return (0);
}

/** Return true iff 'method' is disabled according to the environment. */
static int
event_is_method_disabled(const char *name)
{
	char environment[64];
	int i;

	evutil_snprintf(environment, sizeof(environment), "EVENT_NO%s", name);
	for (i = 8; environment[i] != '\0'; ++i)
		environment[i] = EVUTIL_TOUPPER(environment[i]);
	/* Note that evutil_getenv() ignores the environment entirely if
	 * we're setuid */
	return (evutil_getenv(environment) != NULL);
}


/** Helper for the deferred_cb queue: wake up the event base. */
static void
notify_base_cbq_callback(struct deferred_cb_queue *cb, void *baseptr)
{
	struct event_base *base = baseptr;
	if (EVBASE_NEED_NOTIFY(base))
		evthread_notify_base(base);
}

struct event_base *
event_base_new_with_config(const struct event_config *cfg)
{
	int i;
	struct event_base *base;
	int should_check_environment;

#ifndef _EVENT_DISABLE_DEBUG_MODE
	event_debug_mode_too_late = 1;
#endif
	//分配并清零event_base内存. event_base里的所有成员都会为0  
	if ((base = mm_calloc(1, sizeof(struct event_base))) == NULL) {
		event_warn("%s: calloc", __func__);
		return NULL;
	}
	detect_monotonic();
	gettime(base, &base->event_tv);

	min_heap_ctor(&base->timeheap);
	TAILQ_INIT(&base->eventqueue);
	base->sig.ev_signal_pair[0] = -1;
	base->sig.ev_signal_pair[1] = -1;
	base->th_notify_fd[0] = -1;
	base->th_notify_fd[1] = -1;

	event_deferred_cb_queue_init(&base->defer_queue);
	base->defer_queue.notify_fn = notify_base_cbq_callback;
	base->defer_queue.notify_arg = base;
	if (cfg)
		base->flags = cfg->flags;

	evmap_io_initmap(&base->io);
	evmap_signal_initmap(&base->sigmap);
	event_changelist_init(&base->changelist);

	base->evbase = NULL;

	should_check_environment =
	    !(cfg && (cfg->flags & EVENT_BASE_FLAG_IGNORE_ENV));

	for (i = 0; eventops[i] && !base->evbase; i++) {
		if (cfg != NULL) {
			/* determine if this backend should be avoided */
			if (event_config_is_avoided_method(cfg,
				eventops[i]->name))
				continue;
			if ((eventops[i]->features & cfg->require_features)
			    != cfg->require_features)
				continue;
		}

		/* also obey the environment variables */
		if (should_check_environment &&
		    event_is_method_disabled(eventops[i]->name))
			continue;

		//找到了一个满足条件的多路IO复用函数  
		base->evsel = eventops[i];

		//初始化evbase，后面会说到  
		base->evbase = base->evsel->init(base);
	}

	if (base->evbase == NULL) {
		event_warnx("%s: no event mechanism available",
		    __func__);
		base->evsel = NULL;
		event_base_free(base);
		return NULL;
	}

	if (evutil_getenv("EVENT_SHOW_METHOD"))
		event_msgx("libevent using: %s", base->evsel->name);

	/* allocate a single active event queue */
	if (event_base_priority_init(base, 1) < 0) {
		event_base_free(base);
		return NULL;
	}

	/* prepare for threading */

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	if (EVTHREAD_LOCKING_ENABLED() &&
	    (!cfg || !(cfg->flags & EVENT_BASE_FLAG_NOLOCK))) {
		int r;
		EVTHREAD_ALLOC_LOCK(base->th_base_lock,
		    EVTHREAD_LOCKTYPE_RECURSIVE);
		base->defer_queue.lock = base->th_base_lock;
		EVTHREAD_ALLOC_COND(base->current_event_cond);
		r = evthread_make_base_notifiable(base);
		if (r<0) {
			event_warnx("%s: Unable to make base notifiable.", __func__);
			event_base_free(base);
			return NULL;
		}
	}
#endif

#ifdef WIN32
	//if (cfg && (cfg->flags & EVENT_BASE_FLAG_STARTUP_IOCP))
	//	event_base_start_iocp(base, cfg->n_cpus_hint);
#endif

	return (base);
}//end of event_base_new_with_config()


void
event_base_free(struct event_base *base)
{
	int i, n_deleted=0;
	struct event *ev;
	/* XXXX grab the lock? If there is contention when one thread frees
	 * the base, then the contending thread will be very sad soon. */

	/* event_base_free(NULL) is how to free the current_base if we
	 * made it with event_init and forgot to hold a reference to it. */
	if (base == NULL && current_base)
		base = current_base;
	/* If we're freeing current_base, there won't be a current_base. */
	if (base == current_base)
		current_base = NULL;
	/* Don't actually free NULL. */
	if (base == NULL) {
		event_warnx("%s: no base to free", __func__);
		return;
	}
	/* XXX(niels) - check for internal events first */

#ifdef WIN32
	//event_base_stop_iocp(base);
#endif

	/* threading fds if we have them */
	if (base->th_notify_fd[0] != -1) {
		event_del(&base->th_notify);
		EVUTIL_CLOSESOCKET(base->th_notify_fd[0]);
		if (base->th_notify_fd[1] != -1)
			EVUTIL_CLOSESOCKET(base->th_notify_fd[1]);
		base->th_notify_fd[0] = -1;
		base->th_notify_fd[1] = -1;
		event_debug_unassign(&base->th_notify);
	}

	/* Delete all non-internal events. */
	for (ev = TAILQ_FIRST(&base->eventqueue); ev; ) {
		struct event *next = TAILQ_NEXT(ev, ev_next);
		if (!(ev->ev_flags & EVLIST_INTERNAL)) {
			event_del(ev);
			++n_deleted;
		}
		ev = next;
	}
	while ((ev = min_heap_top(&base->timeheap)) != NULL) {
		event_del(ev);
		++n_deleted;
	}
	for (i = 0; i < base->n_common_timeouts; ++i) {
		struct common_timeout_list *ctl =
		    base->common_timeout_queues[i];
		event_del(&ctl->timeout_event); /* Internal; doesn't count */
		event_debug_unassign(&ctl->timeout_event);
		for (ev = TAILQ_FIRST(&ctl->events); ev; ) {
			struct event *next = TAILQ_NEXT(ev,
			    ev_timeout_pos.ev_next_with_common_timeout);
			if (!(ev->ev_flags & EVLIST_INTERNAL)) {
				event_del(ev);
				++n_deleted;
			}
			ev = next;
		}
		mm_free(ctl);
	}
	if (base->common_timeout_queues)
		mm_free(base->common_timeout_queues);

	for (i = 0; i < base->nactivequeues; ++i) {
		for (ev = TAILQ_FIRST(&base->activequeues[i]); ev; ) {
			struct event *next = TAILQ_NEXT(ev, ev_active_next);
			if (!(ev->ev_flags & EVLIST_INTERNAL)) {
				event_del(ev);
				++n_deleted;
			}
			ev = next;
		}
	}

	if (n_deleted)
		event_debug(("%s: %d events were still set in base",
			__func__, n_deleted));

	if (base->evsel != NULL && base->evsel->dealloc != NULL)
		base->evsel->dealloc(base);

	for (i = 0; i < base->nactivequeues; ++i)
		EVUTIL_ASSERT(TAILQ_EMPTY(&base->activequeues[i]));

	EVUTIL_ASSERT(min_heap_empty(&base->timeheap));
	min_heap_dtor(&base->timeheap);

	mm_free(base->activequeues);

	EVUTIL_ASSERT(TAILQ_EMPTY(&base->eventqueue));

	evmap_io_clear(&base->io);
	evmap_signal_clear(&base->sigmap);
	event_changelist_freemem(&base->changelist);

	EVTHREAD_FREE_LOCK(base->th_base_lock, EVTHREAD_LOCKTYPE_RECURSIVE);
	EVTHREAD_FREE_COND(base->current_event_cond);

	mm_free(base);
}


const char **
event_get_supported_methods(void)
{
	static const char **methods = NULL;
	const struct eventop **method;
	const char **tmp;
	int i = 0, k;

	/* count all methods */
	for (method = &eventops[0]; *method != NULL; ++method) {
		++i;
	}

	/* allocate one more than we need for the NULL pointer */
	tmp = mm_calloc((i + 1), sizeof(char *));
	if (tmp == NULL)
		return (NULL);

	/* populate the array with the supported methods */
	for (k = 0, i = 0; eventops[k] != NULL; ++k) {
		tmp[i++] = eventops[k]->name;
	}
	tmp[i] = NULL;

	if (methods != NULL)
		mm_free((char**)methods);

	methods = tmp;

	return (methods);
}// end  event_get_supported_methods


struct event_config *
event_config_new(void)
{
	struct event_config *cfg = mm_calloc(1, sizeof(*cfg));

	if (cfg == NULL)
		return (NULL);

	TAILQ_INIT(&cfg->entries);

	return (cfg);
}

static void
event_config_entry_free(struct event_config_entry *entry)
{
	if (entry->avoid_method != NULL)
		mm_free((char *)entry->avoid_method);
	mm_free(entry);
}

void
event_config_free(struct event_config *cfg)
{
	struct event_config_entry *entry;

	while ((entry = TAILQ_FIRST(&cfg->entries)) != NULL) {
			TAILQ_REMOVE(&cfg->entries, entry, next);
			event_config_entry_free(entry);
		}
	mm_free(cfg);
}


int
event_config_require_features(struct event_config *cfg,
    int features)
{
	if (!cfg)
		return (-1);
	cfg->require_features = features;
	return (0);
}

int
event_base_priority_init(struct event_base *base, int npriorities)
{
	int i;

	if (N_ACTIVE_CALLBACKS(base) || npriorities < 1
	    || npriorities >= EVENT_MAX_PRIORITIES)
		return (-1);

	if (npriorities == base->nactivequeues)
		return (0);

	if (base->nactivequeues) {
		mm_free(base->activequeues);
		base->nactivequeues = 0;
	}

	/* Allocate our priority queues */
	base->activequeues = (struct event_list *)
	  mm_calloc(npriorities, sizeof(struct event_list));
	if (base->activequeues == NULL) {
		event_warn("%s: calloc", __func__);
		return (-1);
	}
	base->nactivequeues = npriorities;

	for (i = 0; i < base->nactivequeues; ++i) {
		TAILQ_INIT(&base->activequeues[i]);
	}

	return (0);
}

/* Common timeouts are special timeouts that are handled as queues rather than
 * in the minheap.  This is more efficient than the minheap if we happen to
 * know that we're going to get several thousands of timeout events all with
 * the same timeout value.
 *
 * Since all our timeout handling code assumes timevals can be copied,
 * assigned, etc, we can't use "magic pointer" to encode these common
 * timeouts.  Searching through a list to see if every timeout is common could
 * also get inefficient.  Instead, we take advantage of the fact that tv_usec
 * is 32 bits long, but only uses 20 of those bits (since it can never be over
 * 999999.)  We use the top bits to encode 4 bites of magic number, and 8 bits
 * of index into the event_base's aray of common timeouts.
 */

/*
1.对于定时器事件，Libevent采用两种方法处理定时器: 定时器尾队列 和 时间堆
=========================================================================
2.在event结构体中，定义了联合体类型ev_timeout_pos来指明定时器的位置
仅用于定时事件处理器：
当为通过定时器时，ev_next_with_common_timeout指向定时器队列中的位置
当为其它定时器时，min_heap_idx为指向时间堆中的索引
struct event{
	.....
	union {
		TAILQ_ENTRY(event) ev_next_with_common_timeout;
		int min_heap_idx;
	} ev_timeout_pos;
}
=========================================================================
3.有时候使用尾队列（通用定时器）来管理定时器将具有更高的效率。
当需要获取多个具有同一超时值的定时器时，尾队列比时间堆效率更高，
若超时时间相同，在尾队列中，可以通过顺序遍历的方法获取超时的定时器，
而采用时间堆的话，每获取一个定时器需要维护堆，时间复杂度为nlogn。
=========================================================================
4.一个定时器是否是通用定时器取决于其超时值tv_usec,具体由is_common_timeout函数决定：
32位定时值tv->tv_usec高四位为幻数，该定时器的幻数不等于0x50000000时，定时器是时间堆定时器
=========================================================================
5.32位环境下tv_usec long int 32位, 64位环境下tv_usec long int 64位 ,兼容32位系统就当 long int 是32位
=========================================================================
6.tv_usec是微妙,1百万微妙是1秒,但到1秒就进位了,所以最大值 999999，只用了1-20位，因此21-32位可以用于铺助功能
=========================================================================
7.idx = COMMON_TIMEOUT_IDX(tv)
取出tv_usec的21-28位，判断idx < Reactor对象中的n_common_timeouts，
该变量保存了当前Reactor对象中通用定时器队列的长度，
若idx在长度范围内，则为通用定时器，否则为时间堆定时器。
*/

#define MICROSECONDS_MASK       COMMON_TIMEOUT_MICROSECONDS_MASK
#define COMMON_TIMEOUT_IDX_MASK 0x0ff00000
#define COMMON_TIMEOUT_IDX_SHIFT 20
#define COMMON_TIMEOUT_MASK     0xf0000000
#define COMMON_TIMEOUT_MAGIC    0x50000000

#define COMMON_TIMEOUT_IDX(tv) \
	(((tv)->tv_usec & COMMON_TIMEOUT_IDX_MASK)>>COMMON_TIMEOUT_IDX_SHIFT)

//hplsp page255
//本函数用于判定时器是否是通用定时器
//通用定时器是一个尾队列, 传统定时器是最小堆
//返回0表示不是通用定时器
//返回1表示是通用定时器
static inline int
is_common_timeout(const struct timeval *tv,
    const struct event_base *base)
{
	int idx;
	//tv_usec最高4位的值是0x50000000 说明这个定时器是通用定时器
	//如果不是说明，不是通用定时器说明是对定时器
	if ((tv->tv_usec & COMMON_TIMEOUT_MASK) != COMMON_TIMEOUT_MAGIC)
		return 0;
	//从tv_usec21到28位获取通用定时器的位置
	idx = COMMON_TIMEOUT_IDX(tv);
	//通用定时器位置在合理范围内返回真，否则返回假
	return idx < base->n_common_timeouts;
}

/* True iff tv1 and tv2 have the same common-timeout index, or if neither
 * one is a common timeout. */
static inline int
is_same_common_timeout(const struct timeval *tv1, const struct timeval *tv2)
{
	return (tv1->tv_usec & ~MICROSECONDS_MASK) ==
	    (tv2->tv_usec & ~MICROSECONDS_MASK);
}


/** Requires that 'tv' is a common timeout.  Return the corresponding
 * common_timeout_list. */
static inline struct common_timeout_list *
get_common_timeout_list(struct event_base *base, const struct timeval *tv)
{
	return base->common_timeout_queues[COMMON_TIMEOUT_IDX(tv)];
}

/* Add the timeout for the first event in given common timeout list to the
 * event_base's minheap. */
static void
common_timeout_schedule(struct common_timeout_list *ctl,
    const struct timeval *now, struct event *head)
{
	struct timeval timeout = head->ev_timeout;
	timeout.tv_usec &= MICROSECONDS_MASK;
	event_add_internal(&ctl->timeout_event, &timeout, 1);
}

const char *
event_base_get_method(const struct event_base *base)
{
	EVUTIL_ASSERT(base);
	return (base->evsel->name);
}

int
event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, void (*callback)(evutil_socket_t, short, void *), void *arg)
{
	if (!base)
		base = current_base;

	_event_debug_assert_not_added(ev);

	ev->ev_base = base;

	ev->ev_callback = callback;
	ev->ev_arg = arg;
	ev->ev_fd = fd;
	ev->ev_events = events;
	ev->ev_res = 0;
	ev->ev_flags = EVLIST_INIT;
	ev->ev_ncalls = 0;
	ev->ev_pncalls = NULL;

	if (events & EV_SIGNAL) {
		if ((events & (EV_READ|EV_WRITE)) != 0) {
			event_warnx("%s: EV_SIGNAL is not compatible with "
			    "EV_READ or EV_WRITE", __func__);
			return -1;
		}
		ev->ev_closure = EV_CLOSURE_SIGNAL;
	} else {
		if (events & EV_PERSIST) {
			evutil_timerclear(&ev->ev_io_timeout);
			ev->ev_closure = EV_CLOSURE_PERSIST;
		} else {
			ev->ev_closure = EV_CLOSURE_NONE;
		}
	}

	min_heap_elem_init(ev);

	if (base != NULL) {
		/* by default, we put new events into the middle priority */
		ev->ev_pri = base->nactivequeues / 2;
	}

	_event_debug_note_setup(ev);

	return 0;
}// end of event_assign


void
event_debug_unassign(struct event *ev)
{
	_event_debug_assert_not_added(ev);
	_event_debug_note_teardown(ev);

	ev->ev_flags &= ~EVLIST_INIT;
}

int
event_priority_set(struct event *ev, int pri)
{
	_event_debug_assert_is_setup(ev);

	if (ev->ev_flags & EVLIST_ACTIVE)
		return (-1);
	if (pri < 0 || pri >= ev->ev_base->nactivequeues)
		return (-1);

	ev->ev_pri = pri;

	return (0);
}

int
event_add(struct event *ev, const struct timeval *tv)
{
	int res;

	if (EVUTIL_FAILURE_CHECK(!ev->ev_base)) {
		event_warnx("%s: event has no event_base set.", __func__);
		return -1;
	}

	EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	res = event_add_internal(ev, tv, 0);

	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return (res);
}

static int
evthread_notify_base_default(struct event_base *base)
{
	char buf[1];
	int r;
	buf[0] = (char) 0;
#ifdef WIN32
	r = send(base->th_notify_fd[1], buf, 1, 0);
#else
	r = write(base->th_notify_fd[1], buf, 1);
#endif
	return (r < 0 && errno != EAGAIN) ? -1 : 0;
}

#if defined(_EVENT_HAVE_EVENTFD) && defined(_EVENT_HAVE_SYS_EVENTFD_H)
/* Helper callback: wake an event_base from another thread.  This version
 * assumes that you have a working eventfd() implementation. */
static int
evthread_notify_base_eventfd(struct event_base *base)
{
	ev_uint64_t msg = 1;
	int r;
	do {
		r = write(base->th_notify_fd[0], (void*) &msg, sizeof(msg));
	} while (r < 0 && errno == EAGAIN);

	return (r < 0) ? -1 : 0;
}
#endif



static int
evthread_notify_base(struct event_base *base)
{
	EVENT_BASE_ASSERT_LOCKED(base);
	if (!base->th_notify_fn)
		return -1;
	if (base->is_notify_pending)
		return 0;
	base->is_notify_pending = 1;
	return base->th_notify_fn(base);
}

/* Implementation function to add an event.  Works just like event_add,
 * except: 1) it requires that we have the lock.  2) if tv_is_absolute is set,
 * we treat tv as an absolute time, not as an interval to add to the current
 * time */
static inline int
event_add_internal(struct event *ev, const struct timeval *tv,
    int tv_is_absolute)
{
	struct event_base *base = ev->ev_base;
	int res = 0;
	int notify = 0;

	EVENT_BASE_ASSERT_LOCKED(base);
	_event_debug_assert_is_setup(ev);

	event_debug((
		 "event_add: event: %p (fd %d), %s%s%scall %p",
		 ev,
		 (int)ev->ev_fd,
		 ev->ev_events & EV_READ ? "EV_READ " : " ",
		 ev->ev_events & EV_WRITE ? "EV_WRITE " : " ",
		 tv ? "EV_TIMEOUT " : " ",
		 ev->ev_callback));

	EVUTIL_ASSERT(!(ev->ev_flags & ~EVLIST_ALL));

	/*
	 * prepare for timeout insertion further below, if we get a
	 * failure on any step, we should not change any state.
	 */
	if (tv != NULL && !(ev->ev_flags & EVLIST_TIMEOUT)) {
		if (min_heap_reserve(&base->timeheap,
			1 + min_heap_size(&base->timeheap)) == -1)
			return (-1);  /* ENOMEM == errno */
	}

	/* If the main thread is currently executing a signal event's
	 * callback, and we are not the main thread, then we want to wait
	 * until the callback is done before we mess with the event, or else
	 * we can race on ev_ncalls and ev_pncalls below. */
#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	if (base->current_event == ev && (ev->ev_events & EV_SIGNAL)
	    && !EVBASE_IN_THREAD(base)) {
		++base->current_event_waiters;
		EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
	}
#endif

	if ((ev->ev_events & (EV_READ|EV_WRITE|EV_SIGNAL)) &&
	    !(ev->ev_flags & (EVLIST_INSERTED|EVLIST_ACTIVE))) {
		if (ev->ev_events & (EV_READ|EV_WRITE))
			res = evmap_io_add(base, ev->ev_fd, ev);
		else if (ev->ev_events & EV_SIGNAL)
			res = evmap_signal_add(base, (int)ev->ev_fd, ev);
		if (res != -1)
			event_queue_insert(base, ev, EVLIST_INSERTED);
		if (res == 1) {
			/* evmap says we need to notify the main thread. */
			notify = 1;
			res = 0;
		}
	}

	/*
	 * we should change the timeout state only if the previous event
	 * addition succeeded.
	 */
	if (res != -1 && tv != NULL) {
		struct timeval now;
		int common_timeout;

		/*
		 * for persistent timeout events, we remember the
		 * timeout value and re-add the event.
		 *
		 * If tv_is_absolute, this was already set.
		 */
		if (ev->ev_closure == EV_CLOSURE_PERSIST && !tv_is_absolute)
			ev->ev_io_timeout = *tv;

		/*
		 * we already reserved memory above for the case where we
		 * are not replacing an existing timeout.
		 */
		if (ev->ev_flags & EVLIST_TIMEOUT) {
			/* XXX I believe this is needless. */
			if (min_heap_elt_is_top(ev))
				notify = 1;
			event_queue_remove(base, ev, EVLIST_TIMEOUT);
		}

		/* Check if it is active due to a timeout.  Rescheduling
		 * this timeout before the callback can be executed
		 * removes it from the active list. */
		if ((ev->ev_flags & EVLIST_ACTIVE) &&
		    (ev->ev_res & EV_TIMEOUT)) {
			if (ev->ev_events & EV_SIGNAL) {
				/* See if we are just active executing
				 * this event in a loop
				 */
				if (ev->ev_ncalls && ev->ev_pncalls) {
					/* Abort loop */
					*ev->ev_pncalls = 0;
				}
			}

			event_queue_remove(base, ev, EVLIST_ACTIVE);
		}

		gettime(base, &now);

		common_timeout = is_common_timeout(tv, base);
		if (tv_is_absolute) {
			ev->ev_timeout = *tv;
		} else if (common_timeout) {
			struct timeval tmp = *tv;
			tmp.tv_usec &= MICROSECONDS_MASK;
			evutil_timeradd(&now, &tmp, &ev->ev_timeout);
			ev->ev_timeout.tv_usec |=
			    (tv->tv_usec & ~MICROSECONDS_MASK);
		} else {
			evutil_timeradd(&now, tv, &ev->ev_timeout);
		}

		event_debug((
			 "event_add: timeout in %d seconds, call %p",
			 (int)tv->tv_sec, ev->ev_callback));

		event_queue_insert(base, ev, EVLIST_TIMEOUT);
		if (common_timeout) {
			struct common_timeout_list *ctl =
			    get_common_timeout_list(base, &ev->ev_timeout);
			if (ev == TAILQ_FIRST(&ctl->events)) {
				common_timeout_schedule(ctl, &now, ev);
			}
		} else {
			/* See if the earliest timeout is now earlier than it
			 * was before: if so, we will need to tell the main
			 * thread to wake up earlier than it would
			 * otherwise. */
			if (min_heap_elt_is_top(ev))
				notify = 1;
		}
	}

	/* if we are not in the right thread, we need to wake up the loop */
	if (res != -1 && notify && EVBASE_NEED_NOTIFY(base))
		evthread_notify_base(base);

	_event_debug_note_add(ev);

	return (res);
}// end of event_add_internal

int
event_del(struct event *ev)
{
	int res;

	if (EVUTIL_FAILURE_CHECK(!ev->ev_base)) {
		event_warnx("%s: event has no event_base set.", __func__);
		return -1;
	}

	EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	res = event_del_internal(ev);

	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);

	return (res);
}// end of event_del()

static inline int
event_del_internal(struct event *ev)
{
	struct event_base *base;
	int res = 0, notify = 0;

	event_debug(("event_del: %p (fd %d), callback %p",
		ev, (int)ev->ev_fd, ev->ev_callback));

	if (ev->ev_base == NULL)
		return (-1);

	EVENT_BASE_ASSERT_LOCKED(ev->ev_base);

	base = ev->ev_base;
#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	if (base->current_event == ev && !EVBASE_IN_THREAD(base)) {
		++base->current_event_waiters;
		EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
	}
#endif

	//对ev_flags是否合法进行判断
	//代码:3.bit_op1.c
	EVUTIL_ASSERT(!(ev->ev_flags & ~EVLIST_ALL));

	if (ev->ev_events & EV_SIGNAL) {
		if (ev->ev_ncalls && ev->ev_pncalls) {
			*ev->ev_pncalls = 0;
		}
	}

	if (ev->ev_flags & EVLIST_TIMEOUT) {
		/* NOTE: We never need to notify the main thread because of a
		 * deleted timeout event: all that could happen if we don't is
		 * that the dispatch loop might wake up too early.  But the
		 * point of notifying the main thread _is_ to wake up the
		 * dispatch loop early anyway, so we wouldn't gain anything by
		 * doing it.
		 */
		event_queue_remove(base, ev, EVLIST_TIMEOUT);
	}

	if (ev->ev_flags & EVLIST_ACTIVE)
		event_queue_remove(base, ev, EVLIST_ACTIVE);

	if (ev->ev_flags & EVLIST_INSERTED) {
		event_queue_remove(base, ev, EVLIST_INSERTED);
		if (ev->ev_events & (EV_READ|EV_WRITE))
			res = evmap_io_del(base, ev->ev_fd, ev);
		else
			res = evmap_signal_del(base, (int)ev->ev_fd, ev);
		if (res == 1) {
			/* evmap says we need to notify the main thread. */
			notify = 1;
			res = 0;
		}
	}

	/* if we are not in the right thread, we need to wake up the loop */
	if (res != -1 && notify && EVBASE_NEED_NOTIFY(base))
		evthread_notify_base(base);

	_event_debug_note_del(ev);

	return (res);
}// end of event_del_internal()

void
event_active_nolock(struct event *ev, int res, short ncalls)
{
	struct event_base *base;

	event_debug(("event_active: %p (fd %d), res %d, callback %p",
		ev, (int)ev->ev_fd, (int)res, ev->ev_callback));


	/* We get different kinds of events, add them together */
	if (ev->ev_flags & EVLIST_ACTIVE) {
		ev->ev_res |= res;
		return;
	}

	base = ev->ev_base;

	EVENT_BASE_ASSERT_LOCKED(base);

	ev->ev_res = res;

	if (ev->ev_pri < base->event_running_priority)
		base->event_continue = 1;

	if (ev->ev_events & EV_SIGNAL) {
#ifndef _EVENT_DISABLE_THREAD_SUPPORT
		if (base->current_event == ev && !EVBASE_IN_THREAD(base)) {
			++base->current_event_waiters;
			EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
		}
#endif
		ev->ev_ncalls = ncalls;
		ev->ev_pncalls = NULL;
	}

	event_queue_insert(base, ev, EVLIST_ACTIVE);

	if (EVBASE_NEED_NOTIFY(base))
		evthread_notify_base(base);
}//end event_active_nolock


/* Remove 'ev' from 'queue' (EVLIST_...) in base. */
static void
event_queue_remove(struct event_base *base, struct event *ev, int queue)
{
	EVENT_BASE_ASSERT_LOCKED(base);

	if (!(ev->ev_flags & queue)) {
		event_errx(1, "%s: %p(fd %d) not on queue %x", __func__,
			   ev, ev->ev_fd, queue);
		return;
	}
	//说明ev_flags不含有EVLIST_INTERNAL
	//另外一种写法 if (!(ev_flags & EVLIST_INTERNAL))
	//代码:bit_op2.c
	if (~ev->ev_flags & EVLIST_INTERNAL)
		base->event_count--;

	ev->ev_flags &= ~queue;
	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_REMOVE(&base->eventqueue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		base->event_count_active--;
		TAILQ_REMOVE(&base->activequeues[ev->ev_pri],
		    ev, ev_active_next);
		break;
	case EVLIST_TIMEOUT:
		if (is_common_timeout(&ev->ev_timeout, base)) {
			struct common_timeout_list *ctl =
			    get_common_timeout_list(base, &ev->ev_timeout);
			TAILQ_REMOVE(&ctl->events, ev,
			    ev_timeout_pos.ev_next_with_common_timeout);
		} else {
			min_heap_erase(&base->timeheap, ev);
		}
		break;
	default:
		event_errx(1, "%s: unknown queue %x", __func__, queue);
	}
}//end of event_queue_remove()



/* Add 'ev' to the common timeout list in 'ev'. */
static void
insert_common_timeout_inorder(struct common_timeout_list *ctl,
    struct event *ev)
{
	struct event *e;
	/* By all logic, we should just be able to append 'ev' to the end of
	 * ctl->events, since the timeout on each 'ev' is set to {the common
	 * timeout} + {the time when we add the event}, and so the events
	 * should arrive in order of their timeeouts.  But just in case
	 * there's some wacky threading issue going on, we do a search from
	 * the end of 'ev' to find the right insertion point.
	 */
	TAILQ_FOREACH_REVERSE(e, &ctl->events,
	    event_list, ev_timeout_pos.ev_next_with_common_timeout) {
		/* This timercmp is a little sneaky, since both ev and e have
		 * magic values in tv_usec.  Fortunately, they ought to have
		 * the _same_ magic values in tv_usec.  Let's assert for that.
		 */
		EVUTIL_ASSERT(
			is_same_common_timeout(&e->ev_timeout, &ev->ev_timeout));
		if (evutil_timercmp(&ev->ev_timeout, &e->ev_timeout, >=)) {
			TAILQ_INSERT_AFTER(&ctl->events, e, ev,
			    ev_timeout_pos.ev_next_with_common_timeout);
			return;
		}
	}
	TAILQ_INSERT_HEAD(&ctl->events, ev,
	    ev_timeout_pos.ev_next_with_common_timeout);
}



static void
event_queue_insert(struct event_base *base, struct event *ev, int queue)
{
	EVENT_BASE_ASSERT_LOCKED(base);

	if (ev->ev_flags & queue) {
		/* Double insertion is possible for active events */
		if (queue & EVLIST_ACTIVE)
			return;

		event_errx(1, "%s: %p(fd %d) already on queue %x", __func__,
			   ev, ev->ev_fd, queue);
		return;
	}

	if (~ev->ev_flags & EVLIST_INTERNAL)
		base->event_count++;

	ev->ev_flags |= queue;
	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		base->event_count_active++;
		TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_pri],
		    ev,ev_active_next);
		break;
	case EVLIST_TIMEOUT: {
		if (is_common_timeout(&ev->ev_timeout, base)) {
			struct common_timeout_list *ctl =
			    get_common_timeout_list(base, &ev->ev_timeout);
			insert_common_timeout_inorder(ctl, ev);
		} else
			min_heap_push(&base->timeheap, ev);
		break;
	}
	default:
		event_errx(1, "%s: unknown queue %x", __func__, queue);
	}
}//end event_queue_insert




#ifndef _EVENT_DISABLE_MM_REPLACEMENT
static void *(*_mm_malloc_fn)(size_t sz) = NULL;
static void *(*_mm_realloc_fn)(void *p, size_t sz) = NULL;
static void (*_mm_free_fn)(void *p) = NULL;

void *
event_mm_malloc_(size_t sz)
{
    if (_mm_malloc_fn)
        return _mm_malloc_fn(sz);
    else
        return malloc(sz);
}

void *
event_mm_calloc_(size_t count, size_t size)
{
    if (_mm_malloc_fn) {
        size_t sz = count * size;
        void *p = _mm_malloc_fn(sz);
        if (p)
            memset(p, 0, sz);
        return p;
    } else
        return calloc(count, size);
}

char *
event_mm_strdup_(const char *str)
{
    if (_mm_malloc_fn) {
        size_t ln = strlen(str);
        void *p = _mm_malloc_fn(ln+1);
        if (p)
            memcpy(p, str, ln+1);
        return p;
	} else{
		return strdup(str);
	}
}

void *
event_mm_realloc_(void *ptr, size_t sz)
{
	if (_mm_realloc_fn)
		return _mm_realloc_fn(ptr, sz);
	else
		return realloc(ptr, sz);
}

void
event_mm_free_(void *ptr)
{
	if (_mm_free_fn)
		_mm_free_fn(ptr);
	else
		free(ptr);
}

void
event_set_mem_functions(void *(*malloc_fn)(size_t sz),
            void *(*realloc_fn)(void *ptr, size_t sz),
            void (*free_fn)(void *ptr))
{
    _mm_malloc_fn = malloc_fn;
    _mm_realloc_fn = realloc_fn;
    _mm_free_fn = free_fn;
}
#endif //endof _EVENT_DISABLE_MM_REPLACEMENT

#if defined(_EVENT_HAVE_EVENTFD) && defined(_EVENT_HAVE_SYS_EVENTFD_H)
static void
evthread_notify_drain_eventfd(evutil_socket_t fd, short what, void *arg)
{
	ev_uint64_t msg;
	ev_ssize_t r;
	struct event_base *base = arg;

	r = read(fd, (void*) &msg, sizeof(msg));
	if (r<0 && errno != EAGAIN) {
		event_sock_warn(fd, "Error reading from eventfd");
	}
	EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	base->is_notify_pending = 0;
	EVBASE_RELEASE_LOCK(base, th_base_lock);
}
#endif

static void
evthread_notify_drain_default(evutil_socket_t fd, short what, void *arg)
{
	unsigned char buf[1024];
	struct event_base *base = arg;
#ifdef WIN32
	while (recv(fd, (char*)buf, sizeof(buf), 0) > 0)
		;
#else
	while (read(fd, (char*)buf, sizeof(buf)) > 0)
		;
#endif

	EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	base->is_notify_pending = 0;
	EVBASE_RELEASE_LOCK(base, th_base_lock);
}

int
evthread_make_base_notifiable(struct event_base *base)
{
	void (*cb)(evutil_socket_t, short, void *) = evthread_notify_drain_default;
	int (*notify)(struct event_base *) = evthread_notify_base_default;

	/* XXXX grab the lock here? */
	if (!base)
		return -1;

	if (base->th_notify_fd[0] >= 0)
		return 0;

#if defined(_EVENT_HAVE_EVENTFD) && defined(_EVENT_HAVE_SYS_EVENTFD_H)
#ifndef EFD_CLOEXEC
#define EFD_CLOEXEC 0
#endif
	base->th_notify_fd[0] = eventfd(0, EFD_CLOEXEC);
	if (base->th_notify_fd[0] >= 0) {
		evutil_make_socket_closeonexec(base->th_notify_fd[0]);
		notify = evthread_notify_base_eventfd;
		cb = evthread_notify_drain_eventfd;
	}
#endif
#if defined(_EVENT_HAVE_PIPE)
	if (base->th_notify_fd[0] < 0) {
		if ((base->evsel->features & EV_FEATURE_FDS)) {
			if (pipe(base->th_notify_fd) < 0) {
				event_warn("%s: pipe", __func__);
			} else {
				evutil_make_socket_closeonexec(base->th_notify_fd[0]);
				evutil_make_socket_closeonexec(base->th_notify_fd[1]);
			}
		}
	}
#endif

#ifdef WIN32
#define LOCAL_SOCKETPAIR_AF AF_INET
#else
#define LOCAL_SOCKETPAIR_AF AF_UNIX
#endif
	if (base->th_notify_fd[0] < 0) {
		if (evutil_socketpair(LOCAL_SOCKETPAIR_AF, SOCK_STREAM, 0,
			base->th_notify_fd) == -1) {
			event_sock_warn(-1, "%s: socketpair", __func__);
			return (-1);
		} else {
			evutil_make_socket_closeonexec(base->th_notify_fd[0]);
			evutil_make_socket_closeonexec(base->th_notify_fd[1]);
		}
	}

	evutil_make_socket_nonblocking(base->th_notify_fd[0]);

	base->th_notify_fn = notify;

	/*
	  Making the second socket nonblocking is a bit subtle, given that we
	  ignore any EAGAIN returns when writing to it, and you don't usally
	  do that for a nonblocking socket. But if the kernel gives us EAGAIN,
	  then there's no need to add any more data to the buffer, since
	  the main thread is already either about to wake up and drain it,
	  or woken up and in the process of draining it.
	*/
	if (base->th_notify_fd[1] > 0)
		evutil_make_socket_nonblocking(base->th_notify_fd[1]);

	/* prepare an event that we can use for wakeup */
	event_assign(&base->th_notify, base, base->th_notify_fd[0],
				 EV_READ|EV_PERSIST, cb, base);

	/* we need to mark this as internal event */
	base->th_notify.ev_flags |= EVLIST_INTERNAL;
	event_priority_set(&base->th_notify, 0);

	return event_add(&base->th_notify, NULL);
}//end of evthread_make_base_notifiable


#ifndef _EVENT_DISABLE_THREAD_SUPPORT
int
event_global_setup_locks_(const int enable_locks)
{
#ifndef _EVENT_DISABLE_DEBUG_MODE
    EVTHREAD_SETUP_GLOBAL_LOCK(_event_debug_map_lock, 0);
#endif
    if (evsig_global_setup_locks_(enable_locks) < 0) 
        return -1;
    if (evutil_secure_rng_global_setup_locks_(enable_locks) < 0) 
        return -1;
    return 0;
}
#endif //end of _EVENT_DISABLE_THREAD_SUPPORT
