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

//这个求偏移量的功能是Libevent是很有用的。
//不过，Libevent不是直接使用这个宏evutil_offsetof。而是使用宏EVUTIL_UPCAST。
//这个宏EVUTIL_UPCAST的作用是通过成员变量的地址获取其所在的结构体变量地址。比如有下面的结构体
//struct Parent  
//{  
//       struct Children ch;  
//       struct event ev;  
//};  
//假如变量child是struct Children类型指针， 并且它是struct Parent结构体的成员。 
//而且知道了child的地址，现在想获取child所在结构体的struct Parent的地址。
//此时就可以用EVUTIL_UPCAST宏了。如下使用就能转换了。
//
//struct Parent *par = EVUTIL_UPCAST(child, struct Parent, ch);  
//展开宏后，如下  
//struct Parent *par = ((struct Parent *)(((char*)(child))  - evutil_offsetof(struct Parent, ch)));
//
//其中，并不需要ch为struct Parent的第一个成员变量。
//EVUTIL_UPCAST宏的工作原理也是挺简单的，
//成员变量的地址减去其本身相对于所在结构体的偏移量就是所在结构体的起始地址了，再将这个地址强制转换成即可。
//示范代码:11.EVUTIL_UPCAST.c
#define EVUTIL_UPCAST(ptr, type, field)				\
	((type *)(((char*)(ptr)) - evutil_offsetof(type, field)))

int evutil_socket_connect(evutil_socket_t *fd_ptr, struct sockaddr *sa, int socklen);

int evutil_socket_finished_connecting(evutil_socket_t fd);

const char *evutil_getenv(const char *name);

long _evutil_weakrand(void);

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
