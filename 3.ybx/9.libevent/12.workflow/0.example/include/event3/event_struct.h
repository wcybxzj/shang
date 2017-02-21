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


#define EVLIST_TIMEOUT  0x01 //event从属于定时器队列或者时间堆  
#define EVLIST_INSERTED 0x02 //event从属于注册队列  
#define EVLIST_SIGNAL   0x04 //没有使用  
#define EVLIST_ACTIVE   0x08 //event从属于活动队列  
#define EVLIST_INTERNAL 0x10 //该event是内部使用的。信号处理时有用到  
#define EVLIST_INIT 0x80 //event已经被初始化了  
  
/* EVLIST_X_ Private space: 0x1000-0xf000 */  
#define EVLIST_ALL  (0xf000 | 0x9f) //所有标志。这个不能取  

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

struct event_base;
struct event {
    TAILQ_ENTRY(event) ev_active_next;////激活队列  
    TAILQ_ENTRY(event) ev_next;//注册事件队列  

	//hplsp page 255
    /* for managing timeouts */
    union {
		//用于通用定时器(尾队列)找到位置
        TAILQ_ENTRY(event) ev_next_with_common_timeout;
		//用于最小堆定时器找到定时器位置
		//仅用于定时事件处理器(event).EV_TIMEOUT类型
        int min_heap_idx;
    } ev_timeout_pos;

	//对于I/O事件，是文件描述符；对于signal事件，是信号值  
    evutil_socket_t ev_fd;
	
	//所属的event_base 
    struct event_base *ev_base;

    //因为信号和I/O是不能同时设置的。所以可以使用共用体以省内存  
    //在低版本的Libevent，两者是分开的，不在共用体内。 
    union {
		//无论是信号还是IO，都有一个TAILQ_ENTRY的队列。它用于这样的情景:  
		//用户对同一个fd调用event_new多次，并且都使用了不同的回调函数。  
		//每次调用event_new都会产生一个event*。这个xxx_next成员就是把这些  
		//event连接起来的。

        /* used for io events */
        struct {
            TAILQ_ENTRY(event) ev_io_next;
            struct timeval ev_timeout;
        } ev_io;

        /* used by signal events */
        struct {
            TAILQ_ENTRY(event) ev_signal_next;
            short ev_ncalls;//事件就绪执行时，调用ev_callback的次数 
            /* Allows deletes in callback */
            short *ev_pncalls;//指针，指向次数  
        } ev_signal;
    } _ev;

	//记录监听的事件类型 EV_READ EVTIMEOUT之类 
    short ev_events;
	//记录了当前激活事件的类型
    short ev_res;       /* result passed to event callback */
	//可能值为前面的EVLIST_XXX  
    short ev_flags;
	 //本event的优先级。调用event_priority_set设置  
    ev_uint8_t ev_pri;  /* smaller numbers are higher priority */
    ev_uint8_t ev_closure;
	//用于定时器,指定定时器的超时值
    struct timeval ev_timeout;

    /* allows us to adopt for different types of events */
    void (*ev_callback)(evutil_socket_t, short, void *arg);//回调函数
    void *ev_arg;//回调函数 参数
};

TAILQ_HEAD (event_list, event);

#ifdef __cplusplus
}
#endif
#endif

