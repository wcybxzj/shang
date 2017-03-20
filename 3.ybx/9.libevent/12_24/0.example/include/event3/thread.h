#ifndef _EVENT3_THREAD_H
#define _EVENT3_THREAD_H

#ifdef __cpluscplus
extern "C" {
#endif
#include <event3/event-config.h>

#define EVTHREAD_WRITE  0x04
#define EVTHREAD_READ   0x08
#define EVTHREAD_TRY    0x10
#if !defined(_EVENT_DISABLE_THREAD_SUPPORT) || defined(_EVENT_IN_DOXYGEN)
#define EVTHREAD_LOCK_API_VERSION 1

#define EVTHREAD_LOCKTYPE_RECURSIVE 1
#define EVTHREAD_LOCKTYPE_READWRITE 2

struct evthread_lock_callbacks {  
    //版本号，设置为宏EVTHREAD_LOCK_API_VERSION  
    int lock_api_version;  
    //支持的锁类型，有普通锁，递归锁，读写锁三种  
    unsigned supported_locktypes;  
    //分配一个锁变量(指针类型)，因为不同的平台锁变量是不同的类型  
    //所以用这个通用的void*类型  
    void *(*alloc)(unsigned locktype);  
    void (*free)(void *lock, unsigned locktype);  
    int (*lock)(unsigned mode, void *lock);  
    int (*unlock)(unsigned mode, void *lock);  
};  

int evthread_set_lock_callbacks(const struct evthread_lock_callbacks *);

#define EVTHREAD_CONDITION_API_VERSION 1

//问题:
//warning: ‘struct timeval’ declared inside parameter list
//warning: its scope is only this definition or declaration, which is probably not what you want
//在参数列表中定义。
//类型的范围仅仅在本定义或者说明中，也许不是你所想要的。
//出现问题的通常是一个结构体，原因是在其它地方没有发现它的定义。
//通常是没有包含相应的头文件或者头文件位置不对，或者定义放在了引用之后。
//此处必须声明下这个结构体是必须的否则 struct evthread_condition_callbacks 报错
//方法1:
struct timeval;

//方法2:
//或者包含应该包含的头
//#include <sys/time.h>

struct evthread_condition_callbacks {  
    //版本号，设置为EVTHREAD_CONDITION_API_VERSION宏  
    int condition_api_version;  
    void *(*alloc_condition)(unsigned condtype);  
    void (*free_condition)(void *cond);  
    int (*signal_condition)(void *cond, int broadcast);  
    int (*wait_condition)(void *cond, void *lock, 
			const struct timeval *timeout);  
};  

int evthread_set_condition_callbacks(                                                                             
    const struct evthread_condition_callbacks *);

void evthread_set_id_callback(                                                                                    
    unsigned long (*id_fn)(void));

#if defined(_EVENT_HAVE_PTHREADS) || defined(_EVENT_IN_DOXYGEN)                                                   
/*@return 0 on success, -1 on failure. */
int evthread_use_pthreads(void);
/** Defined if Libevent was built with support for evthread_use_pthreads() */
#define EVTHREAD_USE_PTHREADS_IMPLEMENTED 1
#endif

void evthread_enable_lock_debuging(void);                                                                         
#endif//end of _EVENT_DISABLE_THREAD_SUPPORT

struct event_base;
int evthread_make_base_notifiable(struct event_base *base);

#ifdef __cpluscplus
}
#endif

#endif
