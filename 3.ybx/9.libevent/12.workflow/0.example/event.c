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
#include <event3/event_struct.h>
#include "event-internal.h"
#include "defer-internal.h"
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
extern const struct eventop pollops;
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
	&pollops,
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
static int	event_haveevents(struct event_base *);

static int	event_process_active(struct event_base *);

static int	timeout_next(struct event_base *, struct timeval **);
static void	timeout_correct(struct event_base *, struct timeval *);
static void	timeout_process(struct event_base *);

static inline void	event_signal_closure(struct event_base *, struct event *ev);
static inline void	event_persist_closure(struct event_base *, struct event *ev);

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
		use_monotonic = 1;//系统支持monotonic时间

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

	   //如果Libevent所在的系统支持monotonic时间，
	   ///那么Libevent将使用monotonic时间，
	   //也就是说Libevent用于获取系统时间的函数gettime将由monotonic提供时间。
#if defined(_EVENT_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	if (use_monotonic) {
		struct timespec	ts;

		if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
			return (-1);

		tp->tv_sec = ts.tv_sec;
		tp->tv_usec = ts.tv_nsec / 1000;
		//额外的功能 
		//printf("last_update_clock_diff:%d\n",base->last_updated_clock_diff);
		if (base->last_updated_clock_diff + CLOCK_SYNC_INTERVAL
		    < ts.tv_sec) {
			struct timeval tv;
			evutil_gettimeofday(&tv,NULL);
            //tv_clock_diff记录两种时间的时间差  
			evutil_timersub(&tv, tp, &base->tv_clock_diff);
			base->last_updated_clock_diff = ts.tv_sec;
		}

		return (0);
	}else{
		//printf("not use_monotonic\n");
	}
#endif
	//如果所在的系统不支持monotonic时间，那么只能使用evutil_gettimeofday了  
	return (evutil_gettimeofday(tp, NULL));
}
//上面的代码虽然首先是使用cache时间，但实际上event_base结构体的cache时间也是通过调用gettime函数而得到的。
//上面代码也可以看到：如果所在的系统没有提供monotonic时间，那么就只能使用evutil_gettimeofday这个函数提供的系统时间了。
//从上面的分析可知，如果Libevent所在的系统支持monotonic时间，那么根本就不用考虑用户手动修改系统时间这坑爹的事情。
//但如果所在的系统没有支持monotonic时间，那么Libevent就只能使用evutil_gettimeofday获取一个用户能修改的时间。

/** Replace the cached time in 'base' with the current time. */
static inline void
update_time_cache(struct event_base *base)
{
	base->tv_cache.tv_sec = 0;
	if (!(base->flags & EVENT_BASE_FLAG_NO_CACHE_TIME)){
		//printf("使用了缓存时间\n");
	    gettime(base, &base->tv_cache);
	}
}

/** Make 'base' have no current cached time. */
static inline void
clear_time_cache(struct event_base *base)
{
	base->tv_cache.tv_sec = 0;
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
    //测试evthread_lock_callbacks结构中的lock指针函数是否为NULL  
    //即测试Libevent是否已经初始化为支持多线程模式。  
    //由于一开始是用mm_calloc申请内存的，所以该内存区域的值为0  
    //对于th_base_lock变量，目前的值为NULL.  
	if (EVTHREAD_LOCKING_ENABLED() &&
	    (!cfg || !(cfg->flags & EVENT_BASE_FLAG_NOLOCK))) {//配置是支持锁的  
		int r;
		EVTHREAD_ALLOC_LOCK(base->th_base_lock,
		    EVTHREAD_LOCKTYPE_RECURSIVE);//申请一个锁  
		base->defer_queue.lock = base->th_base_lock;
		EVTHREAD_ALLOC_COND(base->current_event_cond);//申请一个条件变量  
		r = evthread_make_base_notifiable(base);//??
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

//可以通过event_base_priority_init函数设置event_base->activequeues数组的个数
int  
event_base_priority_init(struct event_base *base, int npriorities)  
{  
    int i;  
  
    //由N_ACTIVE_CALLBACKS宏可以知道，本函数应该要在event_base_dispatch  
    //函数调用前调用。不然将无法设置。  
    if (N_ACTIVE_CALLBACKS(base) || npriorities < 1  
        || npriorities >= EVENT_MAX_PRIORITIES)  
        return (-1);  
  
    //之前和现在要设置的优先级数是一样的。  
    if (npriorities == base->nactivequeues)  
        return (0);  
  
    //释放之前的，因为N_ACTIVE_CALLBACKS,所以没有active的event。  
    //可以随便mm_free  
    if (base->nactivequeues) {   
        mm_free(base->activequeues);  
        base->nactivequeues = 0;  
    }  
  
    //分配一个优先级数组。  
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

/* Returns true iff we're currently watching any events. */
static int
event_haveevents(struct event_base *base)
{
	/* Caller must hold th_base_lock */
	return (base->virtual_event_count > 0 || base->event_count > 0);
}

/* "closure" function called when processing active signal events */
static inline void
event_signal_closure(struct event_base *base, struct event *ev)
{
	short ncalls;
	int should_break;

	/* Allows deletes to work */
	ncalls = ev->ev_ncalls;
	if (ncalls != 0)
		ev->ev_pncalls = &ncalls;
	//while循环里面会调用用户设置的回调函数。该回调函数可能会执行很久  
    //所以要解锁先.  
	EVBASE_RELEASE_LOCK(base, th_base_lock);
	//如果该信号发生了多次，那么就需要多次执行回调函数
	while (ncalls) {
		ncalls--;
		ev->ev_ncalls = ncalls;
		if (ncalls == 0)
			ev->ev_pncalls = NULL;
		(*ev->ev_callback)(ev->ev_fd, ev->ev_res, ev->ev_arg);

		EVBASE_ACQUIRE_LOCK(base, th_base_lock);
		should_break = base->event_break;
		EVBASE_RELEASE_LOCK(base, th_base_lock);

		if (should_break) {
			if (ncalls != 0)
				ev->ev_pncalls = NULL;
			return;
		}
	}
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

//用来判断一个给定的struct timeval时间， 是否为common-timeout时间。
//在event_add_internal函数中会用之作为判断，
//然后根据判断结果来决定是插入小根堆还是common-timeout，这也就完成了区分。
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

//在common_timeout_schedule函数中，我们可以看到，它将一个event插入到小根堆中了。
//并且也可以看到，代表者不是用户给出的超时event中的一个，而是common_timeout_list结构体的一个event成员。
/* Add the timeout for the first event in given common timeout list to the
 * event_base's minheap. */
static void
common_timeout_schedule(struct common_timeout_list *ctl,
    const struct timeval *now, struct event *head)
{
	struct timeval timeout = head->ev_timeout;
	//清除common-timeout标志 
	timeout.tv_usec &= MICROSECONDS_MASK;
    //用common_timeout_list结构体的一个event成员作为超时event调用event_add_internal  
    //由于已经清除了common-timeout标志，所以这次将插入到小根堆中。  
	event_add_internal(&ctl->timeout_event, &timeout, 1);
}


/* Callback: invoked when the timeout for a common timeout queue triggers.
 * This means that (at least) the first event in that queue should be run,
 * and the timeout should be rescheduled if there are more events. */
//在回调函数中，会手动把用户的超时event激活。于是，用户的超时event就能被处理了。
//由于Libevent这个内部超时event的优先级是最高的，所以在接下来就会处理用户的超时event，
//而无需等到下一轮多路IO复用函数调用返回后。这一点同信号event处理是一样的
static void
common_timeout_callback(evutil_socket_t fd, short what, void *arg)
{
	struct timeval now;
	struct common_timeout_list *ctl = arg;
	struct event_base *base = ctl->base;
	struct event *ev = NULL;
	EVBASE_ACQUIRE_LOCK(base, th_base_lock);
	gettime(base, &now);
	while (1) {
		ev = TAILQ_FIRST(&ctl->events);
		//该超时event还没到超时时间。不要检查其他了。因为是升序的  
		if (!ev || ev->ev_timeout.tv_sec > now.tv_sec ||
		    (ev->ev_timeout.tv_sec == now.tv_sec &&
			(ev->ev_timeout.tv_usec&MICROSECONDS_MASK) > now.tv_usec))
			break;
		//一系列的删除操作。包括从这个超时event队列中删除
		event_del_internal(ev);
		//手动激活超时event。注意，这个ev是用户的超时event
		//由于Libevent这个内部超时event的优先级是最高的，所以在接下来就会处理用户的超时event，
		//而无需等到下一轮多路IO复用函数调用返回后。这一点同信号event是一样的
		event_active_nolock(ev, EV_TIMEOUT, 1);
	}
	//不是NULL，说明该队列还有超时event。将ctl->timeout_event再次加加入到时间堆
	//那么需要再次common_timeout_schedule，进行监听
	if (ev)
		common_timeout_schedule(ctl, &now, ev);
	EVBASE_RELEASE_LOCK(base, th_base_lock);
}

#define MAX_COMMON_TIMEOUTS 256

//使用common-timeout:
//现在来看看怎么使用common-timeout。
//如果要使用common-timeout，就必须把超时event插入到common_timeout_list的events队列中。
//又因为其要求具有相同的超时时长，所以要插入的超时event要和某个common_timeout_list结构体有相同的超时时长。
//所以，我们还是来看一下怎么设置common_timeout_list结构体的超时时长。
//
//不是设置。而是向event_base申请一个具有特定时长的common_timeout_list。
//每申请一个，就会在common_timeout_queues数组中加入一个common_timeout_list元素。
//可以通过event_base_init_common_timeout申请。
//申请后，就可以直接调用event_add把超时event插入到common-timeout中。
//但问题是，common-timeout和小根堆是共存的，event_add又没有第三个参数作为说明，要插入到common-timeout还是小根堆。
//
//common-timeout标志： 
//其实，event_add是根据第二个参数，即超时时长值进行区分的。
//首先有一个基本事实，对一个struct timeval结构体,成员tv_usec的单位是微秒，所以最大也就是999999,
//只需低20比特位就能存储了。但成员tv_usec的类型是int或者long，肯定有32比特位。所以，就有高12比特位是空闲的。
//
//Libevent就是利用那空闲的12个比特位做文章的。这12比特位是高比特位。
//Libevent使用最高的4比特位作为标志位，标志它是一个专门用于common-timeout的时间，下文将这个标志称为common-timeout标志。
//次8比特位用来记录该超时时长在common_timeout_queues数组中的位置，即下标值。
//这也限制了common_timeout_queues数组的长度，最大为2的8次方，即256。

//申请一个时长为duration的common_timeout_list  
//该函数将返回一个具有common-timeout标志的时间。

//该函数只是在event_base的common_timeout_queues数组中申请一个特定超时时长的位置。
//同时该函数也会返回一个struct timeval结构体指针变量，
//该结构体已经被赋予了common-timeout标志。
//以后使用该变量作为event_add的第二个参数，就可以把超时event插入到common-timeout中了。
//不应该也不能自己手动为struct timeval变量加入common-timeout标志。

//该函数中，也给内部的event进行了赋值,设置了回调函数和回调参数。
//要注意的是回调参数是这个common_timeout_list结构体变量指针。
//在回调函数中，有了这个指针，就可以访问events变量，即访问到该结构体上的所有超时event。
//于是就能手动激活这些超时event。
//在Libevent的官方例子中，得到event_base_init_common_timeout的返回值后，
//就把它存放到另外一个struct timeval结构体中。而不是直接使用返回值作为event_add的参数。

//那么怎么得到一个具有common-timeout标志的时间呢？
//其实，还是通过前面说到的event_base_init_common_timeout函数。
//该函数将返回一个具有common-timeout标志的时间
const struct timeval *
event_base_init_common_timeout(struct event_base *base,
    const struct timeval *duration)
{
	int i;
	struct timeval tv;
	const struct timeval *result=NULL;
	struct common_timeout_list *new_ctl;

	EVBASE_ACQUIRE_LOCK(base, th_base_lock);

	//这个时间的微秒位应该进位。用户没有将之进位。比如二进制的103，个位的3应该进位
	//1000000微秒 已经是1秒了应该进位到tv_sec
	//这个弄个tv是因为函数参数const *duration是 *duration是只读
	if (duration->tv_usec > 1000000) {
		//将之进位，因为下面会用到32中的高位12位
		//4位决定是否common_timeout  
		//8位(范围:0-255 ,256个数),表示当前common_timeout_list在二维数组base->common_timeout_queues的那个下标
		memcpy(&tv, duration, sizeof(struct timeval));
		if (is_common_timeout(duration, base))
			tv.tv_usec &= MICROSECONDS_MASK;
		tv.tv_sec += tv.tv_usec / 1000000;
		tv.tv_usec %= 1000000;
		duration = &tv;
	}
	for (i = 0; i < base->n_common_timeouts; ++i) {
		const struct common_timeout_list *ctl =
		    base->common_timeout_queues[i];
		//具有相同的duration， 即之前有申请过这个超时时长。那么就不用分配空间。
		if (duration->tv_sec == ctl->duration.tv_sec &&
		    duration->tv_usec ==
		    (ctl->duration.tv_usec & MICROSECONDS_MASK)) {////要&这个宏，才能是正确的时间
			EVUTIL_ASSERT(is_common_timeout(&ctl->duration, base));
			result = &ctl->duration;
			goto done;
		}
	}

	//达到了最大申请个数，不能再分配了  
	if (base->n_common_timeouts == MAX_COMMON_TIMEOUTS) {
		event_warnx("%s: Too many common timeouts already in use; "
		    "we only support %d per event_base", __func__,
		    MAX_COMMON_TIMEOUTS);
		goto done;
	}

	//之前分配的空间已经用完了，要重新申请空间	
	if (base->n_common_timeouts_allocated == base->n_common_timeouts) {
		int n = base->n_common_timeouts < 16 ? 16 :
		    base->n_common_timeouts*2;
		struct common_timeout_list **newqueues =
		    mm_realloc(base->common_timeout_queues,
			n*sizeof(struct common_timeout_queue *));
		if (!newqueues) {
			event_warn("%s: realloc",__func__);
			goto done;
		}
		base->n_common_timeouts_allocated = n;
		base->common_timeout_queues = newqueues;
	}

    //为该超时时长分配一个common_timeout_list结构体  
	new_ctl = mm_calloc(1, sizeof(struct common_timeout_list));
	if (!new_ctl) {
		event_warn("%s: calloc",__func__);
		goto done;
	}

    //为这个结构体进行一些设置  
	TAILQ_INIT(&new_ctl->events);
	new_ctl->duration.tv_sec = duration->tv_sec;
	// duration->tv_usec | COMMON_TIMEOUT_MAGIC | //为这个时间加入common-timeout标志  
	// (base->n_common_timeouts << COMMON_TIMEOUT_IDX_SHIFT);//加入下标值  
	new_ctl->duration.tv_usec =
	    duration->tv_usec | COMMON_TIMEOUT_MAGIC |
	    (base->n_common_timeouts << COMMON_TIMEOUT_IDX_SHIFT);
	//对timeout_event这个内部event进行赋值。设置回调函数和回调参数。
	evtimer_assign(&new_ctl->timeout_event, base,
	    common_timeout_callback, new_ctl);
	new_ctl->timeout_event.ev_flags |= EVLIST_INTERNAL;//第三种学到对的libevent内部event
	event_priority_set(&new_ctl->timeout_event, 0);//优先级为最高级 ,插入活动队列的优先级
	new_ctl->base = base;
	//放到数组对应的位置上 
	base->common_timeout_queues[base->n_common_timeouts++] = new_ctl;
	result = &new_ctl->duration;

done:
	if (result)
		EVUTIL_ASSERT(is_common_timeout(result, base));

	EVBASE_RELEASE_LOCK(base, th_base_lock);
	return result;
}//end event_base_init_common_timeout


/* Closure function invoked when we're activating a persistent event. */
static inline void
event_persist_closure(struct event_base *base, struct event *ev)
{
	//printf("event_persist_closure\n");
	//printf("ev:%x\n",ev);
	//printf("&base->th_notify:%x\n", &base->th_notify);
	//printf("%d:\n",ev->ev_io_timeout.tv_sec);
	//printf("%d:\n",ev->ev_io_timeout.tv_usec);

	/* reschedule the persistent event if we have a timeout. */
    //在event_add_internal函数中，如果是超时event并且有EV_PERSIST，
	//那么就会把ev_io_timeout设置成 ,用户设置的超时时间(相对时间)。
	//否则为0。即不进入判断体中。说明这个if只用于处理具有EV_PERSIST属性的超时event 
	if (ev->ev_io_timeout.tv_sec || ev->ev_io_timeout.tv_usec) {
		/* If there was a timeout, we want it to run at an interval of
		 * ev_io_timeout after the last time it was _scheduled_ for,
		 * not ev_io_timeout after _now_.  If it fired for another
		 * reason, though, the timeout ought to start ticking _now_. */
		struct timeval run_at, relative_to, delay, now;
		ev_uint32_t usec_mask = 0;
		EVUTIL_ASSERT(is_same_common_timeout(&ev->ev_timeout,
			&ev->ev_io_timeout));
		gettime(base, &now);

		if (is_common_timeout(&ev->ev_timeout, base)) {
			delay = ev->ev_io_timeout;
			usec_mask = delay.tv_usec & ~MICROSECONDS_MASK;
			delay.tv_usec &= MICROSECONDS_MASK;
			if (ev->ev_res & EV_TIMEOUT) {
				relative_to = ev->ev_timeout;
				relative_to.tv_usec &= MICROSECONDS_MASK;
			} else {
				relative_to = now;
			}
		} else {
			//printf("ev_io_timeout.tv_sec:%d tv_usec:%d\n",ev->ev_io_timeout.tv_sec, ev->ev_io_timeout.tv_usec);
			//printf("ev_timeout.tv_sec:%d tv_usec:%d\n",ev->ev_timeout.tv_sec, ev->ev_timeout.tv_usec);
			//delay是用户设置的超时时间。event_add的第二个参数
			delay = ev->ev_io_timeout;

			//1个设置了超时的事件执行有2种可能，有读写事件或者超时
			//下面的判断就是着两种情况
			if (ev->ev_res & EV_TIMEOUT) {
				//printf("event timeout!\n");
				//如果是因为超时而激活，那么下次超时就是本次超时的加上delay时间。  
				relative_to = ev->ev_timeout;
			} else {
				//printf("event read or write!\n");
				//重新计算超时值  
				relative_to = now;
			}
		}

		evutil_timeradd(&relative_to, &delay, &run_at);
        //无论relative是哪个时间，run_at都不应该小于now。  
        //如果小于，则说明是用户手动修改了系统时间，使得gettime()函数获取了一个  
        //之前的时间。比如现在是9点，用户手动调回到7点。
		if (evutil_timercmp(&run_at, &now, <)) {
			/* Looks like we missed at least one invocation due to
			 * a clock jump, not running the event loop for a
			 * while, really slow callbacks, or
			 * something. Reschedule relative to now.
			 */
			evutil_timeradd(&now, &delay, &run_at);
		}
		run_at.tv_usec |= usec_mask;
		//printf("event_add_internal run_at.tv_sec:%d\n", run_at.tv_sec);
		//printf("event_add_internal run_at.tv_usec:%d\n", run_at.tv_usec);
		//把这个event再次添加到event_base中。注意，此时第三个参数为1，说明是一个绝对时间
		event_add_internal(ev, &run_at, 1);
	}
	EVBASE_RELEASE_LOCK(base, th_base_lock);
	(*ev->ev_callback)(ev->ev_fd, ev->ev_res, ev->ev_arg);
}//end event_persist_closure
//这段代码的处理流程是：如果用户指定了EV_PERSIST，那么在event_assign中就记录下来。
//在event_process_active_single_queue函数中会针对永久event进行调用event_persist_closure函数对之进行处理。
//在event_persist_closure函数中，如果是一般的永久event，那么就直接调用该event的回调函数。
//如果是超时永久event，那么就需要再次计算新的超时时间，并将这个event再次插入到event_base中。

//这段代码也指明了，如果一个event因可读而被激活，那么其超时时间就要重新计算。
//而不是之前的那个了。
//也就是说，如果一个event设置了3秒的超时，但1秒后就可读了，
//那么下一个超时值，就要重新计算设置，而不是2秒后。
 
 
//从前面的源码分析也可以得到：如果一个event监听可读的同时也设置了超时值，并且一直没有数据可读，
//最后超时了，那么这个event将会被删除掉，不会再等。


static int
event_process_active_single_queue(struct event_base *base,
    struct event_list *activeq)
{
	struct event *ev;
	int count = 0;

	EVUTIL_ASSERT(activeq != NULL);
	//Libevent在处理内部的那个信号处理event的回调函数时，
	//其实是在event_process_active_single_queue的一个循环里面。
	//因为Libevent内部的信号处理event的优先级最高优先级，
	//并且在前面的将用户信号event插入到队列(即event_queue_insert)，在插入到队列的尾部。
	//所以无论用户的这个信号event的优先级是多少，都是在Libevent的内部信号处理event的后面。
	//所以在遍历上面两个函数的里外两个循环时，肯定会执行到用户的信号event。

	//遍历该优先级的所有event，并处理之  
	for (ev = TAILQ_FIRST(activeq); ev; ev = TAILQ_FIRST(activeq)) {
		//printf("11111111111111\n");
		//如果是永久事件，那么只需从active队列中删除。
		//event_queue_remove后从ev_flags删除EVLIST_ACTIVE,
		//ev_flags只留EVLIST_INIT | EVLIST_INSERTED
		if (ev->ev_events & EV_PERSIST)
			event_queue_remove(base, ev, EVLIST_ACTIVE);
		else//不是的话，那么就要把这个event删除掉。
			event_del_internal(ev);
		if (!(ev->ev_flags & EVLIST_INTERNAL))
			++count;

		event_debug((
			 "event_process_active: event: %p, %s%scall %p",
			ev,
			ev->ev_res & EV_READ ? "EV_READ " : " ",
			ev->ev_res & EV_WRITE ? "EV_WRITE " : " ",
			ev->ev_callback));

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
		base->current_event = ev;
		base->current_event_waiters = 0;
#endif

		switch (ev->ev_closure) {
		case EV_CLOSURE_SIGNAL:
			//printf("EV_CLOSURE_SIGNAL\n");
			event_signal_closure(base, ev);
			break;
		//这个case只对超时event的EV_PERSIST才有用。IO的没有用  
		case EV_CLOSURE_PERSIST:
			//printf("EV_CLOSURE_PERSIST\n");
			event_persist_closure(base, ev);
			break;
		default:
		//默认是EV_CLOSURE_NONE
		//没有设置EV_PERSIST的超时event，就只有一次的监听机会 
		case EV_CLOSURE_NONE:
			//printf("EV_CLOSURE_NONE\n");
			EVBASE_RELEASE_LOCK(base, th_base_lock);
			//调用用户设置的回调函数。  
			(*ev->ev_callback)(
				ev->ev_fd, ev->ev_res, ev->ev_arg);
			break;
		}

		EVBASE_ACQUIRE_LOCK(base, th_base_lock);
#ifndef _EVENT_DISABLE_THREAD_SUPPORT
		base->current_event = NULL;
		if (base->current_event_waiters) {
			base->current_event_waiters = 0;
			EVTHREAD_COND_BROADCAST(base->current_event_cond);
		}
#endif

		if (base->event_break)
			return -1;
		if (base->event_continue)
			break;
	}
	return count;
}//end of  event_process_active_single_queue

static int
event_process_deferred_callbacks(struct deferred_cb_queue *queue, int *breakptr)
{
	int count = 0;
	struct deferred_cb *cb;

#define MAX_DEFERRED 16
	while ((cb = TAILQ_FIRST(&queue->deferred_cb_list))) {
		cb->queued = 0;
		TAILQ_REMOVE(&queue->deferred_cb_list, cb, cb_next);
		--queue->active_count;
		UNLOCK_DEFERRED_QUEUE(queue);

		cb->cb(cb, cb->arg);

		LOCK_DEFERRED_QUEUE(queue);
		if (*breakptr)
			return -1;
		if (++count == MAX_DEFERRED)
			break;
	}
#undef MAX_DEFERRED
	return count;
}


/*
 * Active events are stored in priority queues.  Lower priorities are always
 * process before higher priorities.  Low priority events can starve high
 * priority ones.
 */

static int
event_process_active(struct event_base *base)
{
	/* Caller must hold th_base_lock */
	struct event_list *activeq = NULL;
	int i, c = 0;
	//printf("nactivequeues:%d\n", base->nactivequeues);
	//从高优先级到低优先级遍历优先级数组 
	for (i = 0; i < base->nactivequeues; ++i) {
		//printf("nactivequeues:%d\n", base->nactivequeues);
		if (TAILQ_FIRST(&base->activequeues[i]) != NULL) {
			base->event_running_priority = i;
			activeq = &base->activequeues[i];
			c = event_process_active_single_queue(base, activeq);
			if (c < 0) {
				base->event_running_priority = -1;
				return -1;
			} else if (c > 0){
				break; /* Processed a real event; do not
					* consider lower-priority events */
			}else{
				//printf("event_process_active 是内部event\nn");
			/* If we get here, all of the events we processed
			 * were internal.  Continue. */
			}
		}
	}

	event_process_deferred_callbacks(&base->defer_queue,&base->event_break);
	base->event_running_priority = -1;
	return c;
}


int
event_base_dispatch(struct event_base *event_base)
{
	return (event_base_loop(event_base, 0));
}

const char *
event_base_get_method(const struct event_base *base)
{
	EVUTIL_ASSERT(base);
	return (base->evsel->name);
}

//eivent_base_loop函数内部会进行加锁，
//为这里要对event_base里面的多个队列进行一些数据操作(增删操作)，
//此时要用锁来保护队列不被另外一个线程所破坏。

//evsel->dispatch:
//将调用多路IO复用函数，对event进行监听，
//并且把满足条件的event放到event_base的激活队列中。

//event_process_active:
//遍历这个激活队列的所有event，逐个调用对应的回调函数。
int
event_base_loop(struct event_base *base, int flags)
{
	const struct eventop *evsel = base->evsel;
	struct timeval tv;
	struct timeval *tv_p;
	int res, done, retval = 0;

	/* Grab the lock.  We will release it inside evsel.dispatch, and again
	 * as we invoke user callbacks. */
	EVBASE_ACQUIRE_LOCK(base, th_base_lock);

	if (base->running_loop) {
		event_warnx("%s: reentrant invocation.  Only one event_base_loop"
		    " can run on each event_base at once.", __func__);
		EVBASE_RELEASE_LOCK(base, th_base_lock);
		return -1;
	}

	base->running_loop = 1;

	//要使用cache时间，得在配置event_base时，没有加入EVENT_BASE_FLAG_NO_CACHE_TIME选项
	clear_time_cache(base);

	if (base->sig.ev_signal_added && base->sig.ev_n_signals_added)
		evsig_set_base(base);

	done = 0;

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	base->th_owner_id = EVTHREAD_GET_ID();
#endif

	base->event_gotterm = base->event_break = 0;

	while (!done) {
		base->event_continue = 0;

		/* Terminate the loop if we have been asked to */
		if (base->event_gotterm) {
			break;
		}

		if (base->event_break) {
			break;
		}

		timeout_correct(base, &tv);

		tv_p = &tv;
		if (!N_ACTIVE_CALLBACKS(base) && !(flags & EVLOOP_NONBLOCK)) {
			//printf("first\n");
			//printf("N_ACTIVE_CALLBACKS && !(flags & EVLOOP_NONBLOCK)\n");
			//根据时间堆顶的时间计算evsel->dispatch的最大等待时间(超时值最小)  
			timeout_next(base, &tv_p);

		} else {
			//printf("second\n");
			/*
			 * if we have active events, we just poll new events
			 * without waiting.
			 */
			evutil_timerclear(&tv);
		}

		/* If we have no events, we just exit */
		if (!event_haveevents(base) && !N_ACTIVE_CALLBACKS(base)) {
			//printf("eventqueue not have nad activequeues not have event\n");
			event_debug(("%s: no events registered.", __func__));
			retval = 1;
			goto done;
		}

		/* update last old time */
		//保存系统时间。如果有cache，将保存cache时间。
		gettime(base, &base->event_tv);

        //之所以要在进入dispatch之前清零，是因为进入  
        //dispatch后，可能会等待一段时间。cache就没有意义了。  
        //如果第二个线程此时想add一个event到这个event_base里面，
		//在event_add_internal函数中会调用gettime。如果cache不清零，  
        //那么将会取这个cache时间。这将取一个不准确的时间。 
		clear_time_cache(base);

        //该函数的内部会解锁，然后调用OS提供的的多路IO复用函数。  
        //这个函数退出后，又会立即加锁。这有点像条件变量。
		//printf("%d\n",tv_p->tv_sec);
		//printf("%d\n",tv_p->tv_usec);
		res = evsel->dispatch(base, tv_p);
		//printf("res:%d\n",res);

		if (res == -1) {
			event_debug(("%s: dispatch returned unsuccessfully.",
				__func__));
			retval = -1;
			goto done;
		}

		//强制更新缓存时间
		//因为下面的timeout_process和event_process_active都要用到时间
		update_time_cache(base);

		//处理超时事件，将超时事件插入到激活链表中
		//当从多路IO复用函数返回时，就检查时间小根堆，看有多少个event已经超时了。
		//如果超时了，那就把这个event加入到event_base的激活队列中。
		//并且把这个超时del(删除)掉，这主要是用于非PERSIST 超时event的
		timeout_process(base);

		if (N_ACTIVE_CALLBACKS(base)) {
			//处理激活列表中的event:
			//现在已经完成了将event插入到激活队列中。接下来就是遍历激活数组队列，把所有激活的event都处理即可
			int n = event_process_active(base);
			if ((flags & EVLOOP_ONCE)
			    && N_ACTIVE_CALLBACKS(base) == 0
			    && n != 0)
				done = 1;
		} else if (flags & EVLOOP_NONBLOCK)
			done = 1;
	}
	event_debug(("%s: asked to terminate loop.", __func__));

done:
	base->running_loop = 0;

	EVBASE_RELEASE_LOCK(base, th_base_lock);

	return (retval);
}//end of event_base_loop


//从event_assign函数的名字可以得知它是进行赋值操作的。
//所以它能可以在event被初始化后再次调用。
//不过，初始化后再次调用的话，有些事情要注意。
//如果这个event是用来监听一个信号的，那么就不能让这个event监听读或者写事件。
//原因是其与信号event的实现方法相抵触，具体可以参考《信号event的处理》。
int
event_assign(struct event *ev, struct event_base *base, evutil_socket_t fd, short events, void (*callback)(evutil_socket_t, short, void *), void *arg)
{
	if (!base)
		base = current_base;

	_event_debug_assert_not_added(ev);

	////进行一些赋值和初始化。
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

struct event *
event_new(struct event_base *base, evutil_socket_t fd,\
	short events, void (*cb)(evutil_socket_t, short, void *), void *arg)
{
	struct event *ev;
	ev = mm_malloc(sizeof(struct event));
	if (ev == NULL)
		return (NULL);
	if (event_assign(ev, base, fd, events, cb, arg) < 0) {
		mm_free(ev);
		return (NULL);
	}

	return (ev);
}

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
      
//优先级不能越界  
    if (pri < 0 || pri >= ev->ev_base->nactivequeues)  
        return (-1);  
  
    //pri值越小，其优先级就越高。  
    ev->ev_pri = pri;  
  
    return (0);  
}  

//在调用event_add时设定的超时值是一个时间段(可以认为隔多长时间就触发一次)，
//相对于现在，即调用event_add的时间， 而不是调用event_base_dispatch的时间。
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
    //通知一下，用来唤醒。写一个字节足矣  
#ifdef WIN32  
    r = send(base->th_notify_fd[1], buf, 1, 0);  
#else  
    r = write(base->th_notify_fd[1], buf, 1);  
#endif  
    //即使errno 等于 EAGAIN也无所谓，因为这是由于通信通道已经塞满了  
    //这已经能唤醒主线程了。没必要一定要再写入一个字节  
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
	//确保已经加锁了
	EVENT_BASE_ASSERT_LOCKED(base);
	if (!base->th_notify_fn)
		return -1;

    //写入一个字节，就能使event_base被唤醒。  
    //如果处于未决状态，就没必要写多一个字节  
	if (base->is_notify_pending)
		return 0;
	//通知处于未决状态，当event_base醒过来就变成已决的了。  
	base->is_notify_pending = 1;
	return base->th_notify_fn(base);
}

/* Implementation function to add an event.  Works just like event_add,
 * except: 1) it requires that we have the lock.  2) if tv_is_absolute is set,
 * we treat tv as an absolute time, not as an interval to add to the current
 * time */
//在event_add中，会把第三个参数设为0.使得使用的是相对时间  
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
	//tv不为NULL,就说明是一个超时event,在小根堆中为其留一个位置 
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
		if (ev->ev_events & (EV_READ|EV_WRITE)){
			//printf("evmap_io_add\n");
			res = evmap_io_add(base, ev->ev_fd, ev);
		}
		else if (ev->ev_events & EV_SIGNAL){
			//printf("evmap_signal_add\n");
			res = evmap_signal_add(base, (int)ev->ev_fd, ev);
		}
		if (res != -1){
			//printf("event_queue_insert\n");
			event_queue_insert(base, ev, EVLIST_INSERTED);
		}
		//printf("res:%d\n");
		if (res == 1) {
			/* evmap says we need to notify the main thread. */
			notify = 1;
			res = 0;
		}
	}

	//printf("res:%d\n", res);

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
		//用户把这个event设置成EV_PERSIST。即永久event.  
		//如果没有这样设置的话，那么只会超时一次。设置了，那么就  
		//可以超时多次。那么就要记录用户设置的超时值。
		//例:设置一个io时间带超时时间2秒,这个时间就是个相对时间
		if (ev->ev_closure == EV_CLOSURE_PERSIST && !tv_is_absolute)
			ev->ev_io_timeout = *tv;

		/*
		 * we already reserved memory above for the case where we
		 * are not replacing an existing timeout.
		 */
		//该event之前被加入到超时队列。用户可以对同一个event调用多次event_add  
        //并且可以每次都用不同的超时值。 
		if (ev->ev_flags & EVLIST_TIMEOUT) {
			//printf("min_heap_elt_is_top+event_queue_remove\n");
			/* XXX I believe this is needless. */
            //之前为该event设置的超时值是所有超时中最小的。  
            //从下面的删除可知，会删除这个最小的超时值。此时多路IO复用函数  
            //的超时值参数就已经改变了。 
			//我的理解:
			//执行event_add_internal的是子线程会去通知主线程修改
			//IO复用的超时时间 notify=1 使用 evthread_notify_base(base)来通知
			if (min_heap_elt_is_top(ev))
				notify = 1;//要通知主线程。可能是次线程为这个event调用本函数
            //从超时队列中删除这个event。因为下次会再次加入
            //多次对同一个超时event调用event_add,那么只能保留最后的那个。
			event_queue_remove(base, ev, EVLIST_TIMEOUT);
		}else{
			//printf("debug:not EVLIST_TIMEOUT\n");
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

		//获取现在的时间
		gettime(base, &now);

		 //判断这个时间是否为common-timeout标志
		common_timeout = is_common_timeout(tv, base);
        //虽然用户在event_add时只需用一个相对时间，但实际上在Libevent内部  
        //还是要把这个时间转换成绝对时间。从存储的角度来说，存绝对时间只需  
        //一个变量。而相对时间则需两个，一个存相对值，另一个存参照物。 
		if (tv_is_absolute) {//该参数指明时间是一个绝对时间
			ev->ev_timeout = *tv;
		} else if (common_timeout) {
			struct timeval tmp = *tv;
			//只取真正的时间部分，common-timeout标志位和下标位不要 
			tmp.tv_usec &= MICROSECONDS_MASK;
			//转换成绝对时间  
			evutil_timeradd(&now, &tmp, &ev->ev_timeout);
			//加入标志位
			ev->ev_timeout.tv_usec |=
			    (tv->tv_usec & ~MICROSECONDS_MASK);
		} else {
			//printf("debug:evutil_timeradd\n");
			//参照时间 + 相对时间  ev_timeout存的是绝对时间 
			evutil_timeradd(&now, tv, &ev->ev_timeout);
		}

		event_debug((
			 "event_add: timeout in %d seconds, call %p",
			 (int)tv->tv_sec, ev->ev_callback));

		//将该超时event插入到超时队列或者时间堆
		event_queue_insert(base, ev, EVLIST_TIMEOUT);

		//common-timeout和小根堆的相互配合:
		//通用队列代表插入最小堆:
		//这个代表是 common_timeout_list.timeout_event,单他的时间确实common_timeout的时间
		//common-timeout是采用一个代表的方式进行工作的。
		//所以肯定要有一个代表被插入小根堆中

		//判断要插入的这个超时event是否为这个队列的第一个元素。
		//如果是的话，就说这个特定超时时长队列第一次有超时event要插入。
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
			//本次插入的超时值，是所有超时中最小的。那么此时就需要通知主线程。
			if (min_heap_elt_is_top(ev))
				notify = 1;
		}
	}

	//printf("notify:%d\n", notify);

	/* if we are not in the right thread, we need to wake up the loop */
	//如果代码逻辑中是需要通知的。并且本线程不是主线程。那么就通知主线程 
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
	/* See if we are just active executing this event in a loop */ 
	if (ev->ev_events & EV_SIGNAL) {
		/* Abort loop *///终止循环  
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


//手动激活event：
//除了运行event_base_dispatch死等外界条件把event激活外，
//Libevent还提供了一个API函数event_active， 可以手动地把一个event激活。

//手动激活一个event的原理是：把event插入到激活队列。
//如果执行激活动作的线程不是主线程，那么还要唤醒主线程，
//让主线程及时处理激活event，不再睡眠在多路IO复用函数中。

//由于手动激活一个event是直接把这个event插入到激活队列的，
//所以event的被激活原因(由res参数所指定)可以不是该event监听的事件。
//比如说该event只监听了EV_READ事件，那么可以调用event_active(ev,EV_SIGNAL, 1);
//用信号事件激活该event。

//res是激活的原因，是诸如EV_READ EV_TIMEOUT之类的宏.  
//ncalls只对EV_SIGNAL信号有用，表示信号的次数  
//因为IO事件不讲究次数，信号才讲究次数 
void
event_active(struct event *ev, int res, short ncalls)
{
	if (EVUTIL_FAILURE_CHECK(!ev->ev_base)) {
		event_warnx("%s: event has no event_base set.", __func__);
		return;
	}

	EVBASE_ACQUIRE_LOCK(ev->ev_base, th_base_lock);

	_event_debug_assert_is_setup(ev);

	event_active_nolock(ev, res, ncalls);

	EVBASE_RELEASE_LOCK(ev->ev_base, th_base_lock);
}

//res是激活的原因，是诸如EV_READ EV_TIMEOUT之类的宏.  
//ncalls只对EV_SIGNAL信号有用，表示信号的次数  
//因为IO事件不讲究次数，信号才讲究次数 
void
event_active_nolock(struct event *ev, int res, short ncalls)
{
	struct event_base *base;

	event_debug(("event_active: %p (fd %d), res %d, callback %p",
		ev, (int)ev->ev_fd, (int)res, ev->ev_callback));

	//该event已经是激活状态  
	/* We get different kinds of events, add them together */
	if (ev->ev_flags & EVLIST_ACTIVE) {
		ev->ev_res |= res;
		return;
	}

	base = ev->ev_base;

	EVENT_BASE_ASSERT_LOCKED(base);

	//记录被激活原因。以后会用到 
	ev->ev_res = res;
	
	//这将停止处理低优先级的event。一路回退到event_base_loop中。
	//printf("event_running_priority:%d\n", base->event_running_priority);
	//printf("ev_pri:%d\n",ev->ev_pri);
	if (ev->ev_pri < base->event_running_priority)
		base->event_continue = 1;

	if (ev->ev_events & EV_SIGNAL) {
#ifndef _EVENT_DISABLE_THREAD_SUPPORT
		if (base->current_event == ev && !EVBASE_IN_THREAD(base)) {
			++base->current_event_waiters;
			//由于此时是主线程执行，所以并不会进行这个判断里面
			EVTHREAD_COND_WAIT(base->current_event_cond, base->th_base_lock);
		}
#endif
		ev->ev_ncalls = ncalls;
		ev->ev_pncalls = NULL;
	}
	//将ev插入到激活队列 队尾
	event_queue_insert(base, ev, EVLIST_ACTIVE);

	//调用本函数的线程不是主线程的话，就会通知主线程。使得主线程能赶快处理激活event  
	if (EVBASE_NEED_NOTIFY(base)){
		printf("多线程模式下,base->tid!=当前执行此操作的tid\n");
		evthread_notify_base(base);
	}
}//end event_active_nolock

static int
timeout_next(struct event_base *base, struct timeval **tv_p)
{
	/* Caller must hold th_base_lock */
	struct timeval now;
	struct event *ev;
	struct timeval *tv = *tv_p;
	int res = 0;

	//从时间堆顶元素
    // 堆的首元素具有最小的超时值，这个是小根堆的性质
	ev = min_heap_top(&base->timeheap);

	//堆中没有元素  
	if (ev == NULL) {
		/* if no time-based events are active wait for I/O */
		*tv_p = NULL;
		goto out;
	}

	if (gettime(base, &now) == -1) {
		res = -1;
		goto out;
	}

    // 如果超时时间<=当前时间，堆顶元素已经超时,不能等待，需要立即返回  
    // 因为ev_timeout这个时间是由event_add调用时的绝对时间 + 相对时间。所以ev_timeout是  
    // 绝对时间。可能在调用event_add之后，过了一段时间才调用event_base_diapatch,所以  
    // 现在可能都过了用户设置的超时时间。
	if (evutil_timercmp(&ev->ev_timeout, &now, <=)) {
		evutil_timerclear(tv);
		goto out;
	}

	//如果堆顶元素时间-当前时间=回填的超时间(用于poll或者select或者epoll_wait)
	evutil_timersub(&ev->ev_timeout, &now, tv);

	EVUTIL_ASSERT(tv->tv_sec >= 0);
	EVUTIL_ASSERT(tv->tv_usec >= 0);
	event_debug(("timeout_next: in %d seconds", (int)tv->tv_sec));

out:
	return (res);
}


/*
 * Determines if the time is running backwards by comparing the current time
 * against the last time we checked.  Not needed when using clock monotonic.
 * If time is running backwards, we adjust the firing time of every event by
 * the amount that time seems to have jumped.
 */
static void
timeout_correct(struct event_base *base, struct timeval *tv)
{
	/* Caller must hold th_base_lock. */
	struct event **pev;
	unsigned int size;
	struct timeval off;
	int i;

	//如果系统支持monotonic时间，那么就不需要校准时间了  
	if (use_monotonic)
		return;

	//获取现在的系统时间  
	/* Check if time is running backwards */
	gettime(base, tv);

	//tv大于等于event_tv运行记录时间，说明用户没有往回调系统时间。那么不需要处理  
	if (evutil_timercmp(tv, &base->event_tv, >=)) {
		base->event_tv = *tv;
		return;
	}

	event_debug(("%s: time is running backwards, corrected",
		    __func__));
	//off差值，即用户调小了多少
	//例如:libevent记录了运行在9点, 现在是服务器时间成为了7点
	//9-7=2
	evutil_timersub(&base->event_tv, tv, &off);

    //用户已经修改了OS的系统时间。现在需要对小根堆的所有event  
    //都修改时间。使得之适应新的系统时间  
	/*
	 * We can modify the key element of the node without destroying
	 * the minheap property, because we change every element.
	 */
	pev = base->timeheap.p;
	size = base->timeheap.n;
	for (; size-- > 0; ++pev) {
        //前面已经用off保存了，用户调小了多少。现在只需  
        //将小根堆的所有event的超时时间(绝对时间)都减去这个off即可 
		struct timeval *ev_tv = &(**pev).ev_timeout;//ev_timeout是绝对时间
		evutil_timersub(ev_tv, &off, ev_tv);
	}
	for (i=0; i<base->n_common_timeouts; ++i) {
		struct event *ev;
		struct common_timeout_list *ctl =
		    base->common_timeout_queues[i];
		TAILQ_FOREACH(ev, &ctl->events,
		    ev_timeout_pos.ev_next_with_common_timeout) {
			struct timeval *ev_tv = &ev->ev_timeout;
			ev_tv->tv_usec &= MICROSECONDS_MASK;
			evutil_timersub(ev_tv, &off, ev_tv);
			ev_tv->tv_usec |= COMMON_TIMEOUT_MAGIC |
			    (i<<COMMON_TIMEOUT_IDX_SHIFT);
		}
	}

	/* Now remember what the new time turned out to be. */
	//保存现在的系统时间。以防用户再次修改系统时间
	base->event_tv = *tv;
}//end of timeout_correct

//Libevent用event_base的成员变量event_tv保存用户修改系统时间前的系统时间。
//如果刚保存完，用户就修改系统时间，这样就能精确地计算出用户往回调了多长时间。
//但毕竟Libevent是用户态的库，不能做到用户修改系统时间前的一刻保存系统时间。
//于是Libevent采用多采点的方式，即时不时就保存一次系统时间。
//所以在event_base_loop函数中的while循环体里面会有gettime(base, &base->event_tv);
//这是为了能多采点。
//但这个while循环里面还会执行多路IO复用函数和处理被激活event的回调函数(这个回调函数执行多久也是个未知数)。
//这两个函数的执行需要的时间可能会比较长，如果用户刚才是在执行完这两个函数之后修改系统时间，
//那么event_tv保存的时间就不怎么精确了。这也是没有办法的啊！！唉！！
//===============================================================================
//timeout_process函数就是处理超了时的event
//把超时了的event，放到激活队列中。并且，其激活原因设置为EV_TIMEOUT 
//===============================================================================
/* Activate every event whose timeout has elapsed. */
static void
timeout_process(struct event_base *base)
{
	/* Caller must hold lock. */
	struct timeval now;
	struct event *ev;
	//ev->ev_timeout存的是绝对时间  
	//超时时间比此刻时间大，说明该event还没超时。那么余下的小根堆元素更不用检查了。  
	if (min_heap_empty(&base->timeheap)) {
		//printf("min_heap_empty\n");
		return;
	}else{
		//printf("min_heap_not_empty\n");
	}

	gettime(base, &now);

	//遍历小根堆的元素。之所以不是只取堆顶那一个元素，是因为当主线程调用多路IO复用函数  
	//进入等待时，次线程可能添加了多个超时值更小的event
	while ((ev = min_heap_top(&base->timeheap))) {
		if (evutil_timercmp(&ev->ev_timeout, &now, >)){
			//printf("time_out break!\n");
			break;
		}

		/* delete this event from the I/O queues */
        //下面说到的del是等同于调用event_del.把event从这个event_base中(所有的队列都)  
        //删除。event_base不再监听之。  
        //这里是timeout_process函数。所以对于有超时的event，才会被del掉。  
        //对于有EV_PERSIST选项的event，在处理激活event的时候，会再次添加进event_base的。  
        //这样做的一个好处就是，再次添加的时候，又可以重新计算该event的超时时间(绝对时间)。
		event_del_internal(ev);

		event_debug(("timeout_process: call %p",
			 ev->ev_callback));
        //把这个event加入到event_base的激活队列中。  
        //event_base的激活队列又有该event了。所以如果该event是EV_PERSIST的，是可以  
        //再次添加进该event_base的  
		event_active_nolock(ev, EV_TIMEOUT, 1);
	}
}//end timeout_process



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
			//printf("work\n");
			min_heap_erase(&base->timeheap, ev);
		}
		break;
	default:
		event_errx(1, "%s: unknown queue %x", __func__, queue);
	}
}//end of event_queue_remove()



//inorder说明是有序的 数组
/* Add 'ev' to the common timeout list in 'ev'. */
static void
insert_common_timeout_inorder(struct common_timeout_list *ctl,
    struct event *ev)
{
	struct event *e;

	//多线程情况下使用尾队列链表的知识点:
	//虽然有相同超时时长，但超时时间却是 超时时长 + 调用event_add的时间。  
    //所以是在不同的时间触发超时的。它们根据绝对超时时间，升序排在队列中。  
    //一般来说，直接插入队尾即可。因为后插入的，绝对超时时间肯定大。  
    //但由于线程抢占的原因，可能一个线程在evutil_timeradd(&now, &tmp, &ev->ev_timeout);  
    //执行完，还没来得及插入，就被另外一个线程抢占了。而这个线程也是要插入一个  
    //common-timeout的超时event。这样就会发生：超时时间小的反而后插入。  
    //所以要从后面开始遍历队列，寻找一个合适的地方。

	//这个考虑很好，但是在event_add中已经lock->th_base_lock,这个做法是否是多虑
	
	/* By all logic, we should just be able to append 'ev' to the end of
	 * ctl->events, since the timeout on each 'ev' is set to {the common
	 * timeout} + {the time when we add the event}, and so the events
	 * should arrive in order of their timeeouts.  But just in case
	 * there's some wacky threading issue going on, we do a search from
	 * the end of 'ev' to find the right insertion point.
	 */
	//具体:
	//倒叙取每个尾队列元素并且还要用要插入的event和取出的event进行比较
	//达到避免出现时间循序错误的情况
	TAILQ_FOREACH_REVERSE(e, &ctl->events,
	    event_list, ev_timeout_pos.ev_next_with_common_timeout) {
		/* This timercmp is a little sneaky, since both ev and e have
		 * magic values in tv_usec.  Fortunately, they ought to have
		 * the _same_ magic values in tv_usec.  Let's assert for that.
		 */
		EVUTIL_ASSERT(
			is_same_common_timeout(&e->ev_timeout, &ev->ev_timeout));
		//保证尾队列按照绝对时间升序排列
		if (evutil_timercmp(&ev->ev_timeout, &e->ev_timeout, >=)) {
			TAILQ_INSERT_AFTER(&ctl->events, e, ev,
			    ev_timeout_pos.ev_next_with_common_timeout);
			return;//插入后就返回  
		}
	}
	//在队列头插入，只会发生在前面的寻找都没有寻找到的情况下
	TAILQ_INSERT_HEAD(&ctl->events, ev,
	    ev_timeout_pos.ev_next_with_common_timeout);
}

//这个函数的主要作为是把event加入到对应的队列中。
//event_new-->event_assign后event->ev_flags是EVLIST_INIT
//event_add 是把event加入到eventqueue队列,event->ev_flags是EVLIST_INIT|EVLIST_INSERTED
//poll_dispatch是把event加入到activequeues队列,event->ev_flags是EVLIST_INIT|EVLIST_INSERTED|EVLIST_ACTIVE
static void
event_queue_insert(struct event_base *base, struct event *ev, int queue)
{
	EVENT_BASE_ASSERT_LOCKED(base);
	//printf("event_queue_insert\n");
	//if (ev->ev_flags & EVLIST_INTERNAL) {
	//	printf("EVLIST_INTERNAL\n");
	//}
	//if (ev->ev_flags & EVLIST_INSERTED) {
	//	printf("EVLIST_INSERTED\n");
	//}

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

	//如果 event_add_internal中调用 queue是 EVLIST_INSERTED
	//event结构体的ev_flags变量为EVLIST_INIT | EVLIST_INSERTED。
	ev->ev_flags |= queue;
	switch (queue) {
	case EVLIST_INSERTED:
		TAILQ_INSERT_TAIL(&base->eventqueue, ev, ev_next);
		break;
	case EVLIST_ACTIVE:
		//将event插入到对应对应优先级的激活队列中  
		base->event_count_active++;
		TAILQ_INSERT_TAIL(&base->activequeues[ev->ev_pri],
		    ev,ev_active_next);
		break;
	case EVLIST_TIMEOUT: {
		 //根据时间向event_base获取对应的common_timeout_list
		if (is_common_timeout(&ev->ev_timeout, base)) {
			struct common_timeout_list *ctl =
			    get_common_timeout_list(base, &ev->ev_timeout);
			//插入到通用超时队列
			insert_common_timeout_inorder(ctl, ev);
		} else{
			min_heap_push(&base->timeheap, ev);
				//printf("------------------------\n");
				//if (ev->ev_flags & EVLIST_INTERNAL){
				//	printf("EVLIST_INTERNAL\n");
				//}
				//if (ev->ev_flags & EVLIST_INSERTED){
				//	printf("EVLIST_INSERTED\n");
				//}
		}
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
  
    //读完fd的所有数据，免得再次被唤醒  
#ifdef WIN32  
    while (recv(fd, (char*)buf, sizeof(buf), 0) > 0)  
        ;  
#else  
    while (read(fd, (char*)buf, sizeof(buf)) > 0)  
        ;  
#endif  
  
    EVBASE_ACQUIRE_LOCK(base, th_base_lock);  
    //修改之，使得其不再是未决的了。当然这也能让其他线程可以再次唤醒值。参看evthread_notify_base函数  
    base->is_notify_pending = 0;  
    EVBASE_RELEASE_LOCK(base, th_base_lock);  
}  

int
evthread_make_base_notifiable(struct event_base *base)
{
    //默认event回调函数和默认的通知函数  
	void (*cb)(evutil_socket_t, short, void *) = evthread_notify_drain_default;
	int (*notify)(struct event_base *) = evthread_notify_base_default;

	/* XXXX grab the lock here? */
	if (!base)
		return -1;
	//th_notify_fd[0]被初始化为-1,如果>=0,就说明已经被设置过了 
	if (base->th_notify_fd[0] >= 0)
		return 0;

#if defined(_EVENT_HAVE_EVENTFD) && defined(_EVENT_HAVE_SYS_EVENTFD_H)
#ifndef EFD_CLOEXEC
#define EFD_CLOEXEC 0
#endif
    //Libevent优先使用eventfd，但eventfd的通信机制和其他的不一样。所以  
    //要专门为eventfd创建通知函数和event回调函数
	base->th_notify_fd[0] = eventfd(0, EFD_CLOEXEC);
	if (base->th_notify_fd[0] >= 0) {
		evutil_make_socket_closeonexec(base->th_notify_fd[0]);
		notify = evthread_notify_base_eventfd;
		cb = evthread_notify_drain_eventfd;
	}
#endif
#if defined(_EVENT_HAVE_PIPE)
	//<0，说明之前的通知方式没有用上
	if (base->th_notify_fd[0] < 0) {
		//有些多路IO复用函数并不支持文件描述符。如果不支持，那么就不能使用这种  
        //通知方式。有关这个的讨论.查看http://blog.csdn.net/luotuo44/article/details/38443569  
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

	//无论哪种通信机制，都要使得读端不能阻塞  
	evutil_make_socket_nonblocking(base->th_notify_fd[0]);

	//设置回调函数 
	base->th_notify_fn = notify;

	//debug
	//printf("%d\n",base->th_notify_fd[1]);

	/*
	  Making the second socket nonblocking is a bit subtle, given that we
	  ignore any EAGAIN returns when writing to it, and you don't usally
	  do that for a nonblocking socket. But if the kernel gives us EAGAIN,
	  then there's no need to add any more data to the buffer, since
	  the main thread is already either about to wake up and drain it,
	  or woken up and in the process of draining it.
	*/
    //同样为了让写端不阻塞。虽然，如果同时出现大量需要notify的操作，会塞满通信通道。  
    //本次的notify会没有写入到通信通道中(已经变成非阻塞了)。但这无所谓，因为目的是  
    //唤醒主线程，通信通道有数据就肯定能唤醒。  
	if (base->th_notify_fd[1] > 0)
		evutil_make_socket_nonblocking(base->th_notify_fd[1]);

	/* prepare an event that we can use for wakeup */
    //该函数的作用等同于event_new。实际上event_new内部也是调用event_assign函数完成工作的  
    //函数cb作为这个event的回调函数  
	event_assign(&base->th_notify, base, base->th_notify_fd[0],
				 EV_READ|EV_PERSIST, cb, base);

	//标明是内部使用的  
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
