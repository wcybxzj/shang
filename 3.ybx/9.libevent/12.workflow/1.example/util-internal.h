#ifndef _EVENT_UTIL_INTERNAL_H
#define _EVENT_UTIL_INTERNAL_H
#include <event3/event-config.h>
#include <errno.h>

/* For EVUTIL_ASSERT */
#include "log-internal.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <event3/util.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _EVUTIL_NIL_STMT ((void)0)

int EVUTIL_ISALPHA(char c);
int EVUTIL_ISALNUM(char c);
int EVUTIL_ISSPACE(char c);
int EVUTIL_ISDIGIT(char c);
int EVUTIL_ISXDIGIT(char c);
int EVUTIL_ISPRINT(char c);
int EVUTIL_ISLOWER(char c);
int EVUTIL_ISUPPER(char c);
char EVUTIL_TOUPPER(char c);
char EVUTIL_TOLOWER(char c);

const char *evutil_getenv(const char *name);

#ifndef WIN32
#define EVUTIL_ERR_RW_RETRIABLE(e)				\
	((e) == EINTR || (e) == EAGAIN)

#define EVUTIL_ERR_CONNECT_RETRIABLE(e)			\
	((e) == EINTR || (e) == EINPROGRESS)

#define EVUTIL_ERR_ACCEPT_RETRIABLE(e)			\
	((e) == EINTR || (e) == EAGAIN || (e) == ECONNABORTED)

#define EVUTIL_ERR_CONNECT_REFUSED(e)					\
	((e) == ECONNREFUSED)

#else

#define EVUTIL_ERR_RW_RETRIABLE(e)					\
	((e) == WSAEWOULDBLOCK ||					\
	    (e) == WSAEINTR)

#define EVUTIL_ERR_CONNECT_RETRIABLE(e)					\
	((e) == WSAEWOULDBLOCK ||					\
	    (e) == WSAEINTR ||						\
	    (e) == WSAEINPROGRESS ||					\
	    (e) == WSAEINVAL)

#define EVUTIL_ERR_ACCEPT_RETRIABLE(e)			\
	EVUTIL_ERR_RW_RETRIABLE(e)

#define EVUTIL_ERR_CONNECT_REFUSED(e)					\
	((e) == WSAECONNREFUSED)

#endif //define WIN32



#if defined(__GNUC__) && __GNUC__ >= 3
#define EVUTIL_UNLIKELY(p) __builtin_expect(!!(p),0)
#else
#define EVUTIL_UNLIKELY(p) (p)
#endif

#ifdef NDEBUG
#define EVUTIL_ASSERT(cond) _EVUTIL_NIL_CONDITION(cond)
#define EVUTIL_FAILURE_CHECK(cond) 0
#else
#define EVUTIL_ASSERT(cond)                     \
    do {                                \
        if (EVUTIL_UNLIKELY(!(cond))) {             \
            event_errx(_EVENT_ERR_ABORT,            \
                "%s:%d: Assertion %s failed in %s",     \
                __FILE__,__LINE__,#cond,__func__);      \
            /* In case a user-supplied handler tries to */  \
            /* return control to us, log and abort here. */ \
            (void)fprintf(stderr,               \
                "%s:%d: Assertion %s failed in %s",     \
                __FILE__,__LINE__,#cond,__func__);      \
            abort();                    \
        }                           \
    } while (0)
#define EVUTIL_FAILURE_CHECK(cond) EVUTIL_UNLIKELY(cond)
#endif


long evutil_tv_to_msec(const struct timeval *tv);

#ifdef __cplusplus
}
#endif



#endif
