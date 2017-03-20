#ifndef _EVSIGNAL_H_
#define _EVSIGNAL_H_

#ifndef evutil_socket_t
#include <event3/util.h>
#endif
#include <signal.h>

typedef void (*ev_sighandler_t)(int);

struct evsig_info {  
    //用于监听socketpair读端的event. ev_signal_pair[1]为读端  
    struct event ev_signal;  
    //socketpair  
    evutil_socket_t ev_signal_pair[2];  
    //用来标志是否已经将ev_signal这个event加入到event_base中了  
    int ev_signal_added;  
    //用户一共要监听多少个信号  
    int ev_n_signals_added;  
  
    //数组。用户可能已经设置过某个信号的信号捕抓函数。但  
    //Libevent还是要为这个信号设置另外一个信号捕抓函数，  
    //此时，就要保存用户之前设置的信号捕抓函数。当用户不要  
    //监听这个信号时，就能够恢复用户之前的捕抓函数。  
    //因为是有多个信号，所以得用一个数组保存。  
#ifdef _EVENT_HAVE_SIGACTION  
    struct sigaction **sh_old;   
#else//保存的是捕抓函数的函数指针，又因为是数组。所以是二级指针  
    ev_sighandler_t **sh_old;   
#endif  
    /* Size of sh_old. */  
    int sh_old_max; //数组的长度  
};
int evsig_init(struct event_base *);
void evsig_dealloc(struct event_base *);

void evsig_set_base(struct event_base *base);
#endif
