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

//http://blog.csdn.net/luotuo44/article/details/38780157
//Libevent定义了一系列位宽的整型，如下图：
/*
Type			Width	Signed		   Maximum			   Minimum
ev_uint64_t		   64      No		EV_UINT64_MAX			  0
ev_int64_t		   64      Yes		EV_INT64_MAX		EV_INT64_MIN 
ev_uint32_t 	   32      No		EV_UINT32_MAX			  0 
ev_int32_t		   32      Yes		EV_INT32_MAX		EV_INT32_MIN
ev_uint16_t 	   16      No		EV_UINT16_MAX			  0
ev_int16_t  	   16      Yes		EV_INT16_MAX		EV_INT16_MIN
ev_uint8_t		    8      No		EV_UINT8_MAX			  0
ev_int8_t		    8      Yes		EV_INT8_MAX			EV_INT8_MIN
*/

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

#ifdef _EVENT_HAVE_UINT32_T
#define ev_uint32_t uint32_t
#define ev_int32_t int32_t
#elif defined(WIN32)
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif _EVENT_SIZEOF_LONG == 4
#define ev_uint32_t unsigned long
#define ev_int32_t signed long
#elif _EVENT_SIZEOF_INT == 4
#define ev_uint32_t unsigned int
#define ev_int32_t signed int
#elif defined(_EVENT_IN_DOXYGEN)
#define ev_uint32_t ...
#define ev_int32_t ...
#else
#error "No way to define ev_uint32_t"
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


//偏移类型：
//Libevent定义了ev_off_t作为兼容的偏移类型。其实现也很简单。
#ifdef WIN32
#define ev_off_t ev_int64_t
#else
#define ev_off_t off_t
#endif


/*
类型				32位系统	64位系统
char					1			1
int						4			4
short					2			2
long					4			8
long long				8			8
float					4			4
double					8			8
long double				8			16
指针类型				4			8
*/

//EV_UINT64_MAX是需要使用位操作才能得到的。
//因为对于UL（unsigned long）类型说，是可移植的最大值了。
//因为对于32位的OS来说，long类型位宽是32位的，64位的OS，long是64位的。
//对于0xffffffffUL这个只有32位的字面值来说保证了可移植性。
//接着把其强制转换成ev_uint64_t类型，此时就有了64位宽，无论是在32位的系统还是64位的系统。
//然后再利用位操作达到目的。
#define EV_UINT64_MAX ((((ev_uint64_t)0xffffffffUL) << 32) | 0xffffffffUL)
#define EV_INT64_MAX  ((((ev_int64_t) 0x7fffffffL) << 32) | 0xffffffffL)
#define EV_INT64_MIN  ((-EV_INT64_MAX) - 1)
#define EV_UINT32_MAX ((ev_uint32_t)0xffffffffUL)
#define EV_INT32_MAX  ((ev_int32_t) 0x7fffffffL)
#define EV_INT32_MIN  ((-EV_INT32_MAX) - 1)
#define EV_UINT16_MAX ((ev_uint16_t)0xffffUL)
#define EV_INT16_MAX  ((ev_int16_t) 0x7fffL)
#define EV_INT16_MIN  ((-EV_INT16_MAX) - 1)
#define EV_UINT8_MAX  255
#define EV_INT8_MAX   127
#define EV_INT8_MIN   ((-EV_INT8_MAX) - 1)

//指针类型：
//intptr_t是一个很重要的类型，特别是在64位系统中。
//如果你要对两个指针进行运算，最好是先将这两个指针转换成intptr_t类型，然后才进行运算。
//因为在一些64位系统中，int还是32位，而指针类型为64位，所以两个指针相减，
//其结果对于32位的int来说，可能会溢出。
//为了兼容，Libevent定义了兼容的intptr_t类型。

//在event-config.h中，_EVENT_SIZEOF_VOID_P被定义成sizeof(void* )，即一个指针类型的字节数。
//一般来说，在32位系统中，为4字节； 在64位系统中，为8字节
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

#define EV_SSIZE_MIN ((-EV_SSIZE_MAX) - 1)

#ifdef _EVENT_ssize_t
#define ev_ssize_t _EVENT_ssize_t
#else
#define ev_ssize_t ssize_t
#endif

//socklen_t类型：
//在Berkeley套接字中，有一些函数的参数类型是socklen_t类型，你不能传一个int或者size_t过去。
//但在Windows系统中，又没有这样的一个类型。比如bind函数。
//在使用Berkeley套接字的系统上，该函数的第三个参数为socklen_t，而在Windows系统上，该参数的类型只是简单的int。
//为此，Libevent定义了一个兼容的ev_socklen_t类型。其实现为：
#ifdef WIN32
#define ev_socklen_t int
#elif defined(_EVENT_socklen_t)
#define ev_socklen_t _EVENT_socklen_t
#else
#define ev_socklen_t socklen_t
#endif

//#ifdef WIN32
//#define evutil_socket_t intptr_t
//#else
#define evutil_socket_t int
//#endif

int evutil_socketpair(int d, int type, int protocol, evutil_socket_t sv[2]);
int evutil_make_socket_nonblocking(evutil_socket_t sock);
int evutil_make_listen_socket_reuseable(evutil_socket_t sock);
int evutil_make_socket_closeonexec(evutil_socket_t sock);
int evutil_closesocket(evutil_socket_t sock);
#define EVUTIL_CLOSESOCKET(s) evutil_closesocket(s)

#ifdef WIN32
#define EVUTIL_SOCKET_ERROR() WSAGetLastError()
#define EVUTIL_SET_SOCKET_ERROR(errcode)		\
	do { WSASetLastError(errcode); } while (0)
int evutil_socket_geterror(evutil_socket_t sock);
const char *evutil_socket_error_to_string(int errcode);
#elif defined(_EVENT_IN_DOXYGEN)
#define EVUTIL_SOCKET_ERROR() ...
#define EVUTIL_SET_SOCKET_ERROR(errcode) ...
#define evutil_socket_geterror(sock) ...
#define evutil_socket_error_to_string(errcode) ...
#else
#define EVUTIL_SOCKET_ERROR() (errno)
#define EVUTIL_SET_SOCKET_ERROR(errcode)		\
		do { errno = (errcode); } while (0)
#define evutil_socket_geterror(sock) (errno)
#define evutil_socket_error_to_string(errcode) (strerror(errcode))
#endif

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
	(												\
	 ((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec)					\
	 )

//结构体偏移量：
//这个函数的功能主要是求结构体成员在结构体中的偏移量。定义如下：
//其中，type表示结构体名称，field表示成员名称。
//可以看到，Libevent还是优先使用所在系统本身提供的offsetof函数。
//Libevent自己实现的版本也是很巧妙的。它用(type*)0来让编译器认为有个结构体，它的起始地址为0。
//这样，编译器给field所在的地址就是编译器给field安排的偏移量。
/** Replacement for offsetof on platforms that don't define it. */
#ifdef offsetof
#define evutil_offsetof(type, field) offsetof(type, field)
#else
#define evutil_offsetof(type, field) ((off_t)(&((type *)0)->field))
#endif

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
