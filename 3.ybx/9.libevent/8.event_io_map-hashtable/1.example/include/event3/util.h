#ifndef _EVENT3_UTIL_H
#define _EVENT3_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef _EVENT_HAVE_STDINT_H
#include <stdint.h>
#elif defined(_EVENT_HAVE_INTTYPES_H)
#include <inttypes.h>
#endif
#ifdef _EVENT_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef _EVENT_HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef _MSC_VER
#include <BaseTsd.h>
#endif
#include <stdarg.h>
#ifdef _EVENT_HAVE_NETDB_H
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <netdb.h>
#endif

#include <sys/socket.h>

#ifdef _EVENT_HAVE_UINT64_T
#define ev_uint64_t uint64_t
#define ev_int64_t int64_t
#elif defined(WIN32)
#define ev_uint64_t unsigned __int64
#define ev_int64_t signed __int64
#elif _EVENT_SIZEOF_LONG_LONG == 8
#define ev_uint64_t unsigned long long
#define ev_int64_t long long
#elif _EVENT_SIZEOF_LONG == 8
#define ev_uint64_t unsigned long
#define ev_int64_t long
#elif defined(_EVENT_IN_DOXYGEN)
#define ev_uint64_t ...
#define ev_int64_t ...
#else
#error "No way to define ev_uint64_t"
#endif

#ifdef _EVENT_HAVE_UINT16_T
#define ev_uint16_t uint16_t
#define ev_int16_t  int16_t
#elif defined(WIN32)
#define ev_uint16_t unsigned short
#define ev_int16_t  signed short
#elif _EVENT_SIZEOF_INT == 2
#define ev_uint16_t unsigned int
#define ev_int16_t  signed int
#elif _EVENT_SIZEOF_SHORT == 2
#define ev_uint16_t unsigned short
#define ev_int16_t  signed short
#elif defined(_EVENT_IN_DOXYGEN)
#define ev_uint16_t ...
#define ev_int16_t ...
#else
#error "No way to define ev_uint16_t"
#endif

#ifdef _EVENT_HAVE_UINT8_T
#define ev_uint8_t uint8_t
#define ev_int8_t int8_t
#elif defined(_EVENT_IN_DOXYGEN)
#define ev_uint8_t ...
#define ev_int8_t ...
#else
#define ev_uint8_t unsigned char
#define ev_int8_t signed char
#endif

#ifdef _EVENT_HAVE_UINTPTR_T
#define ev_uintptr_t uintptr_t
#define ev_intptr_t intptr_t
#elif _EVENT_SIZEOF_VOID_P <= 4
#define ev_uintptr_t ev_uint32_t
#define ev_intptr_t ev_int32_t
#elif _EVENT_SIZEOF_VOID_P <= 8
#define ev_uintptr_t ev_uint64_t
#define ev_intptr_t ev_int64_t
#elif defined(_EVENT_IN_DOXYGEN)
#define ev_uintptr_t ...
#define ev_intptr_t ...
#else
#error "No way to define ev_uintptr_t"
#endif

#ifdef _EVENT_ssize_t
#define ev_ssize_t _EVENT_ssize_t
#else
#define ev_ssize_t ssize_t
#endif


//#ifdef WIN32
//#define evutil_socket_t intptr_t
//#else
#define evutil_socket_t int
//#endif

int evutil_socketpair(int d, int type, int protocol, evutil_socket_t sv[2]);
int evutil_make_socket_nonblocking(evutil_socket_t sock);
int evutil_make_socket_closeonexec(evutil_socket_t sock);
int evutil_closesocket(evutil_socket_t sock);
#define EVUTIL_CLOSESOCKET(s) evutil_closesocket(s)

#define EVUTIL_SOCKET_ERROR() (errno)
#define EVUTIL_SET_SOCKET_ERROR(errcode)		\
		do { errno = (errcode); } while (0)
#define evutil_socket_geterror(sock) (errno)
#define evutil_socket_error_to_string(errcode) (strerror(errcode))

#ifdef _EVENT_HAVE_TIMERADD                                                                                                         
#define evutil_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define evutil_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#else
#define evutil_timeradd(tvp, uvp, vvp)                  \
    do {                                \
        (vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;      \
        (vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
        if ((vvp)->tv_usec >= 1000000) {            \
            (vvp)->tv_sec++;                \
            (vvp)->tv_usec -= 1000000;          \
        }                           \
    } while (0)
#define evutil_timersub(tvp, uvp, vvp)                  \
    do {                                \
        (vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;      \
        (vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;   \
        if ((vvp)->tv_usec < 0) {               \
            (vvp)->tv_sec--;                \
            (vvp)->tv_usec += 1000000;          \
        }                           \
    } while (0)
#endif /* !_EVENT_HAVE_HAVE_TIMERADD */

#ifdef _EVENT_HAVE_TIMERCLEAR
#define evutil_timerclear(tvp) timerclear(tvp)
#else
#define	evutil_timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0
#endif

/** Return true iff the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >. */
#define	evutil_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))

/** Replacement for gettimeofday on platforms that lack it. */
#ifdef _EVENT_HAVE_GETTIMEOFDAY
#define evutil_gettimeofday(tv, tz) gettimeofday((tv), (tz))
#else
struct timezone;                                                                                                                    
int evutil_gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

/** Replacement for snprintf to get consistent behavior on platforms for                                          
    which the return value of snprintf does not conform to C99.
 */
int evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 4)))
#endif
;


/** Replacement for vsnprintf to get consistent behavior on platforms for
    which the return value of snprintf does not conform to C99.
 */
int evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 0)))
#endif
;


#ifdef __cplusplus
}
#endif

#endif
