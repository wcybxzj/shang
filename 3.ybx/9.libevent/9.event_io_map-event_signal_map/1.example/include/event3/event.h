#ifndef _EVENT3_EVENT_H_
#define _EVENT3_EVENT_H_ 

#include <event3/event-config.h>
#ifdef _EVENT_HAVE_SYS_TYPES_H 
#include <sys/types.h> 
#endif 
#ifdef _EVENT_HAVE_SYS_TIME_H 
#include <sys/time.h> 
#endif 
 
#include <stdio.h> 
 
#include <event3/util.h>

enum event_method_feature {  
    //支持边沿触发  
    EV_FEATURE_ET = 0x01,  
    //添加、删除、或者确定哪个事件激活这些动作的时间复杂度都为O(1)  
    //select、poll是不能满足这个特征的.epoll则满足  
    EV_FEATURE_O1 = 0x02,  
    //支持任意的文件描述符，而不能仅仅支持套接字  
    EV_FEATURE_FDS = 0x04  
};  

enum event_base_config_flag {
	EVENT_BASE_FLAG_NOLOCK = 0x01,
	EVENT_BASE_FLAG_IGNORE_ENV = 0x02,
	EVENT_BASE_FLAG_STARTUP_IOCP = 0x04,
	EVENT_BASE_FLAG_NO_CACHE_TIME = 0x08,
	EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10
};

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

#define EV_TIMEOUT	0x01
#define EV_READ		0x02
#define EV_WRITE	0x04
#define EV_SIGNAL	0x08
#define EV_PERSIST	0x10
#define EV_ET       0x20

#if !defined(_EVENT_DISABLE_MM_REPLACEMENT) || defined(_EVENT_IN_DOXYGEN)
//自定义内存管理函数借口给用户来设置
void event_set_mem_functions(
    void *(*malloc_fn)(size_t sz), 
    void *(*realloc_fn)(void *ptr, size_t sz), 
    void (*free_fn)(void *ptr));
#define EVENT_SET_MEM_FUNCTIONS_IMPLEMENTED
#endif


#endif
