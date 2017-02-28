#ifndef _EVENT3_EVENT_H_
#define _EVENT3_EVENT_H_ 

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
 
#include <stdio.h> 
 
#include <event3/util.h>

struct event_base
#ifdef _EVENT_IN_DOXYGEN
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

struct event
#ifdef _EVENT_IN_DOXYGEN
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

struct event_config
#ifdef _EVENT_IN_DOXYGEN
{/*Empty body so that doxygen will generate documentation here.*/}
#endif
;

void event_debug_unassign(struct event *);
struct event_base *event_base_new(void);
int event_base_dispatch(struct event_base *);
const char *event_base_get_method(const struct event_base *);
const char **event_get_supported_methods(void);
struct event_config *event_config_new(void);
void event_config_free(struct event_config *cfg);

enum event_method_feature {  
    //支持边沿触发  
    EV_FEATURE_ET = 0x01,  
    //添加、删除、或者确定哪个事件激活这些动作的时间复杂度都为O(1)  
    //select、poll是不能满足这个特征的.epoll则满足  
    EV_FEATURE_O1 = 0x02,  
    //支持任意的文件描述符，而不能仅仅支持套接字  
    EV_FEATURE_FDS = 0x04  
};  

/*
EVENT_BASE_FLAG_NOLOCK：不为event_base设置锁，可以省去event_base加锁和释放锁的时间，但是对于多线程操作，并不安全。
EVENT_BASE_FLAG_IGNORE_ENV：当调用后端的方法时并不检查EVENT_*环境变量，用这个选项时注意，会导致程序和Libevent库之间调试麻烦
EVENT_BASE_FLAG_STARTUP_IOCP：仅对windows有效，通知Libevent在启动的时候就是用必要的IOCP 派发逻辑，而不是在需要时才用IOCP派发逻辑。
EVENT_BASE_FLAG_NO_CACHE_TIME：每次超时回调函数调用后检测当前时间，而不是准备调用超时回调函数前检测。
EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST：
通知libevent，如果后台使用epoll模型，使用changelist是安全的，
epoll-changelist 能够避免在两次派发之间如果同一个fd的状态有改变， 多次不必要的系统调用。
换句话说在派发消息中间，如果同一个fd多次改动，那么最终只调用一次系统 调用。
如果后台不是epoll模型，那么这个选项是没什么影响。
同样可以设置EVENT_EPOLL_USE_CHANGELIST 环境变量达到这个目的。
EVENT_BASE_FLAG_PRECISE_TIMER：
libevent默认使用快速的定时器机制，设置这个选项后，如果有一个慢速的但是定时器精度高的机制，那么就会切换到这个机制。
如果没有这个机制，那么这个选项没什么影响。
*/
enum event_base_config_flag {
	EVENT_BASE_FLAG_NOLOCK = 0x01,
	EVENT_BASE_FLAG_IGNORE_ENV = 0x02,
	EVENT_BASE_FLAG_STARTUP_IOCP = 0x04,
	EVENT_BASE_FLAG_NO_CACHE_TIME = 0x08,
	EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10
};

int event_config_require_features(struct event_config *cfg, int feature);

struct event_base *event_base_new_with_config(const struct event_config *);
void event_base_free(struct event_base *);

#define EVENT_LOG_DEBUG 0
#define EVENT_LOG_MSG   1
#define EVENT_LOG_WARN  2
#define EVENT_LOG_ERR   3

#define _EVENT_LOG_DEBUG EVENT_LOG_DEBUG
#define _EVENT_LOG_MSG EVENT_LOG_MSG
#define _EVENT_LOG_WARN EVENT_LOG_WARN
#define _EVENT_LOG_ERR EVENT_LOG_ERR

typedef void (*event_log_cb)(int severity, const char *msg);
void event_set_log_callback(event_log_cb cb);

typedef void (*event_fatal_cb)(int err);
void event_set_fatal_callback(event_fatal_cb cb);

//阻塞直到有一个活跃的event，然后执行完活跃事件的回调就退出。
#define EVLOOP_ONCE	0x01
//不阻塞，检查哪个事件准备好，调用优先级最高的那一个，然后退出。
#define EVLOOP_NONBLOCK	0x02

int event_base_loop(struct event_base *, int);


//EV_TIMEOUT:
//这个标志表示某超时时间流逝后事件成为激活的。
//构造事件的时候，EV_TIMEOUT标志是被忽略的：
//可以在添加事件的时候设置超时，也可以不设置。
//超时发生时，回调函数的what参数将带有这个标志。
//
//EV_READ:
//表示指定的文件描述符已经就绪，可以读取的时候，事件将成为激活的。
//
//EV_WRITE:
//表示指定的文件描述符已经就绪，可以写入的时候，事件将成为激活的。
//
//EV_SIGNAL:
//用于实现信号检测，请看下面的“构造信号事件”节。
//
//EV_PERSIST:
//表示事件是“持久的”，请看下面的“关于事件持久性”节。
//
//EV_ET:
//表示如果底层的event_base后端支持边沿触发事件，则事件应该是边沿触发的。
//这个标志影响EV_READ和EV_WRITE的语义。


#define EV_TIMEOUT	0x01
#define EV_READ		0x02
#define EV_WRITE	0x04
#define EV_SIGNAL	0x08
#define EV_PERSIST	0x10
#define EV_ET       0x20

#define evtimer_assign(ev, b, cb, arg) \
	event_assign((ev), (b), -1, 0, (cb), (arg))

#define evsignal_new(b, x, cb, arg)				\
		event_new((b), (x), EV_SIGNAL|EV_PERSIST, (cb), (arg))

typedef void (*event_callback_fn)(evutil_socket_t, short, void *);

struct event *event_new(struct event_base *, evutil_socket_t, short, event_callback_fn, void *);


int event_assign(struct event *, struct event_base *, evutil_socket_t, short, event_callback_fn, void *);
int event_add(struct event *ev, const struct timeval *timeout);
int event_del(struct event *);
void event_active(struct event *ev, int res, short ncalls);

#define EVENT_MAX_PRIORITIES 256
int	event_base_priority_init(struct event_base *, int);
int	event_priority_set(struct event *, int);
const struct timeval *event_base_init_common_timeout(struct event_base *base,
    const struct timeval *duration);

#if !defined(_EVENT_DISABLE_MM_REPLACEMENT) || defined(_EVENT_IN_DOXYGEN)
//自定义内存管理函数借口给用户来设置
void event_set_mem_functions(
    void *(*malloc_fn)(size_t sz), 
    void *(*realloc_fn)(void *ptr, size_t sz), 
    void (*free_fn)(void *ptr));
#define EVENT_SET_MEM_FUNCTIONS_IMPLEMENTED
#endif


#ifdef __cplusplus
}
#endif

#endif
