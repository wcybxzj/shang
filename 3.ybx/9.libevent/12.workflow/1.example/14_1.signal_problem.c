#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"


void sig_cb(int fd, short events, void *arg)  
{  
    printf("in the sig_cb\n");  
}  
  
void signal_handle(int sig)  
{  
    printf("catch1111 the sig %d\n", sig);  
}  


//终端2:
//kill -s SIGUSR1 11765

//终端1:
//[root@web11 1.example]# ./14.signal 
//pid = 11765
//begin
//catch the sig 10
//catch the sig 10

int main()  
{  
  
    struct event_base *base = event_base_new();  
  
    struct event *ev = evsignal_new(base, SIGUSR1, sig_cb, NULL);  
    event_add(ev, NULL);  //里面有signal或者sigaction

	//覆盖同一个信号的之前设置，造成发来的信号无法在libewvent中进行相应
    signal(SIGUSR1, signal_handle);  
  
    printf("pid = %d\n", getpid());  

    printf("begin\n");  
    event_base_dispatch(base);  
    printf("end\n");  
  
    return 0;  
}  
