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

void sig_cb2(int fd, short events, void *arg)  
{  
    printf("in the sig_cb2\n");  
}  

//测试1:普通流程
//终端1:
///14_2.signal_ok 
//pid = 22284
//begin
//in the sig_cb
//终端2:
//kill -USR1 22284
void test1()
{
    struct event_base *base = event_base_new();  
	
    struct event *ev = evsignal_new(base, SIGUSR1, sig_cb, NULL);  
    event_add(ev, NULL);

    printf("pid = %d\n", getpid());  

    printf("begin\n");  
    event_base_dispatch(base);  
    printf("end\n");  
}

//测试2:测试同一个信号添加二次
//event_signal_map->信号尾队列
//event_io_map->io尾队列
//终端1:
//./14_2.signal_ok 
//pid = 22312
//begin
//in the sig_cb
//in the sig_cb2
//终端2:
//kill -USR1 22312
void test2()
{
    struct event_base *base = event_base_new();  
	
    struct event *ev = evsignal_new(base, SIGUSR1, sig_cb, NULL);  
    event_add(ev, NULL);

    struct event *ev2 = evsignal_new(base, SIGUSR1, sig_cb2, NULL);  
    event_add(ev2, NULL);

    printf("pid = %d\n", getpid());  

    printf("begin\n");  
    event_base_dispatch(base);  
    printf("end\n");  
}



int main()  
{  
	//test1();
	test2();
  
    return 0;  
}  
