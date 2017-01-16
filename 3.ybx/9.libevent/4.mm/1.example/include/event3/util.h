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


#define evutil_socket_t int

#define EVUTIL_SOCKET_ERROR() (errno)                                                                                                               
#define EVUTIL_SET_SOCKET_ERROR(errcode)        \
        do { errno = (errcode); } while (0)
#define evutil_socket_geterror(sock) (errno)
#define evutil_socket_error_to_string(errcode) (strerror(errcode))

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
