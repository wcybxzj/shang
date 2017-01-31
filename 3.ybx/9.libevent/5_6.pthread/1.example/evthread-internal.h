#ifndef _EVTHREAD_INTERNAL_H
#define _EVTHREAD_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif
#include <event3/thread.h>
#include <event3/event-config.h>
#include "util-internal.h"

#if ! defined(_EVENT_DISABLE_THREAD_SUPPORT) && defined(EVTHREAD_EXPOSE_STRUCTS)                                                

#elif ! defined(_EVENT_DISABLE_THREAD_SUPPORT)                                                                                  

unsigned long _evthreadimpl_get_id(void);
int _evthreadimpl_is_lock_debugging_enabled(void);
void *_evthreadimpl_lock_alloc(unsigned locktype);
void _evthreadimpl_lock_free(void *lock, unsigned locktype);
int _evthreadimpl_lock_lock(unsigned mode, void *lock);
int _evthreadimpl_lock_unlock(unsigned mode, void *lock);
void *_evthreadimpl_cond_alloc(unsigned condtype);
void _evthreadimpl_cond_free(void *cond);
int _evthreadimpl_cond_signal(void *cond, int broadcast);
int _evthreadimpl_cond_wait(void *cond, void *lock, const struct timeval *tv);
int _evthreadimpl_locking_enabled(void);

#define EVTHREAD_GET_ID() _evthreadimpl_get_id()
#define EVBASE_IN_THREAD(base)              \
    ((base)->th_owner_id == _evthreadimpl_get_id())
#define EVBASE_NEED_NOTIFY(base)             \
    ((base)->running_loop &&             \
        ((base)->th_owner_id != _evthreadimpl_get_id()))

#define EVTHREAD_ALLOC_LOCK(lockvar, locktype)      \
    ((lockvar) = _evthreadimpl_lock_alloc(locktype))

#define EVTHREAD_FREE_LOCK(lockvar, locktype)               \
    do {                                \
        void *_lock_tmp_ = (lockvar);               \
        if (_lock_tmp_)                     \
            _evthreadimpl_lock_free(_lock_tmp_, (locktype)); \
    } while (0)

/** Acquire a lock. */
#define EVLOCK_LOCK(lockvar,mode)                   \
    do {                                \
        if (lockvar)                       \
            _evthreadimpl_lock_lock(mode, lockvar);     \
    } while (0)

/** Release a lock */
#define EVLOCK_UNLOCK(lockvar,mode)                 \
    do {                                \
        if (lockvar)                        \
            _evthreadimpl_lock_unlock(mode, lockvar);   \
    } while (0)

/** Lock an event_base, if it is set up for locking.  Acquires the lock
    in the base structure whose field is named 'lockvar'. */
#define EVBASE_ACQUIRE_LOCK(base, lockvar) do {             \
        EVLOCK_LOCK((base)->lockvar, 0);            \
    } while (0)

/** Unlock an event_base, if it is set up for locking. */
#define EVBASE_RELEASE_LOCK(base, lockvar) do {             \
        EVLOCK_UNLOCK((base)->lockvar, 0);          \
    } while (0)


#define EVLOCK_ASSERT_LOCKED(lock)                  \
    do {                                \
        if ((lock) && _evthreadimpl_is_lock_debugging_enabled()) { \
            EVUTIL_ASSERT(_evthread_is_debug_lock_held(lock)); \
        }                           \
    } while (0)

static inline int EVLOCK_TRY_LOCK(void *lock);
static inline int
EVLOCK_TRY_LOCK(void *lock)
{
    if (lock) {
        int r = _evthreadimpl_lock_lock(EVTHREAD_TRY, lock);
        return !r;
    } else {
        /* Locking is disabled either globally or for this thing;
         * of course we count as having the lock. */
        return 1;
    }
}

#define EVTHREAD_ALLOC_COND(condvar)                    \
    do {                                \
        (condvar) = _evthreadimpl_cond_alloc(0);        \
    } while (0)

#define EVTHREAD_FREE_COND(cond)                    \
    do {                                \
        if (cond)                       \
            _evthreadimpl_cond_free((cond));        \
    } while (0)

#define EVTHREAD_COND_SIGNAL(cond)                  \
    ( (cond) ? _evthreadimpl_cond_signal((cond), 0) : 0 )

#define EVTHREAD_COND_BROADCAST(cond)                   \
    ( (cond) ? _evthreadimpl_cond_signal((cond), 1) : 0 )

/*Returns 0 for * success, -1 for failure. */
#define EVTHREAD_COND_WAIT(cond, lock)                  \
    ( (cond) ? _evthreadimpl_cond_wait((cond), (lock), NULL) : 0 )

#define EVTHREAD_COND_WAIT_TIMED(cond, lock, tv)            \
    ( (cond) ? _evthreadimpl_cond_wait((cond), (lock), (tv)) : 0 )

#define EVTHREAD_LOCKING_ENABLED()      \
    (_evthreadimpl_locking_enabled())

#else /*_EVENT_DISABLE_THREAD_SUPPORT */                                                                                       

#endif //! defined(_EVENT_DISABLE_THREAD_SUPPORT) && defined(EVTHREAD_EXPOSE_STRUCTS)


#if ! defined(_EVENT_DISABLE_THREAD_SUPPORT)
void *evthread_setup_global_lock_(void *lock_, unsigned locktype, 
		int enable_locks);

#define EVTHREAD_SETUP_GLOBAL_LOCK(lockvar, locktype)           \
    do {                                \
        lockvar = evthread_setup_global_lock_(lockvar,      \
            (locktype), enable_locks);              \
        if (!lockvar) {                     \
            event_warn("Couldn't allocate %s", #lockvar);   \
            return -1;                  \
        }                           \
    } while (0);

int event_global_setup_locks_(const int enable_locks);
int evsig_global_setup_locks_(const int enable_locks);
int evutil_secure_rng_global_setup_locks_(const int enable_locks);

#endif//_EVENT_DISABLE_THREAD_SUPPORT

#ifdef __cplusplus
}
#endif

#endif //endof _EVTHREAD_INTERNAL_H
