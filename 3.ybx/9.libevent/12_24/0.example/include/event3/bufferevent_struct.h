#ifndef _EVENT2_BUFFEREVENT_STRUCT_H_
#define _EVENT2_BUFFEREVENT_STRUCT_H_

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
/* For struct event */
#include <event3/event_struct.h>

struct event_watermark {
	size_t low;
	size_t high;
};

//bufferevent结构体：
//bufferevent其实也就是在event_base的基础上再进行一层封装，
//其本质还是离不开event和event_base，从bufferevent的结构体就可以看到这一点。
//bufferevent结构体中有两个event，分别用来监听同一个fd的可读事件和可写事件。
//为什么不用一个event同时监听可读和可写呢？这是因为监听可写是困难的，下面会说到原因。
//读者也可以自问一下，自己之前有没有试过用最原始的event监听一个fd的可写。
//由于socket 是全双工的，所以在bufferevent结构体中，也有两个evbuffer成员，分别是读缓冲区和写缓冲区。
struct bufferevent {  
    struct event_base *ev_base;  
    //操作结构体，成员有一些函数指针。类似struct eventop结构体  
    const struct bufferevent_ops *be_ops;  
    struct event ev_read;//读事件event  
    struct event ev_write;//写事件event  
  
    struct evbuffer *input;//读缓冲区  
  
    struct evbuffer *output; //写缓冲区  
  
    struct event_watermark wm_read;//读水位  
    struct event_watermark wm_write;//写水位  
  
      
    bufferevent_data_cb readcb;//可读时的回调函数指针  
    bufferevent_data_cb writecb;//可写时的回调函数指针  
    bufferevent_event_cb errorcb;//错误发生时的回调函数指针  
    void *cbarg;//回调函数的参数  
  
    struct timeval timeout_read;//读事件event的超时值  
    struct timeval timeout_write;//写事件event的超时值  
  
    /** Events that are currently enabled: currently EV_READ and EV_WRITE 
        are supported. */  
    short enabled;  
};  

#ifdef __cplusplus
}
#endif

#endif /* _EVENT2_BUFFEREVENT_STRUCT_H_ */

