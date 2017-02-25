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

void cmd_cb(int fd, short events, void *arg)  
{  
    char buf[1024];  
    printf("in the cmd_cb\n");  
    read(fd, buf, sizeof(buf));  
}  
  
void cmd_cb5(int fd, short events, void *arg)  
{  
    char buf[1024];  
    printf("in the cmd_cb5\n");  
}  

//测试1:io event EV_READ并且设置了超时
//结果:超时后执行回调删除event
//./16_1.timeout_event 
//in the cmd_cb
//a
void func1()
{
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, STDIN_FILENO,  
                                     EV_READ , cmd_cb, NULL);  
  
    struct timeval tv = {2, 0};
    event_add(cmd_ev, &tv); //超时  
  
    event_base_dispatch(base);  
}

//测试2:io event EV_READ并且没有设置超时
//结果:阻塞等待事件
//./16_1.timeout_event 
//a
//in the cmd_cb
void func2()
{
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, STDIN_FILENO,  
                                     EV_READ , cmd_cb, NULL);  
  
    event_add(cmd_ev, NULL);//没超时
  
    event_base_dispatch(base);  
}

//测试3:io event EV_READ|EV_PRESIST并且设置了超时
void func3()
{
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, STDIN_FILENO,  
                                     EV_READ | EV_PERSIST, cmd_cb, NULL);  
  
    struct timeval tv = {2, 0};
    event_add(cmd_ev, &tv); //超时  
  
    event_base_dispatch(base);  
}


//测试4:io event EV_READ|EV_PRESIST并且没有设置超时
void func4()
{
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, STDIN_FILENO,  
                                     EV_READ | EV_PERSIST, cmd_cb, NULL);  
  
    event_add(cmd_ev, NULL);//没超时
  
    event_base_dispatch(base);  
}

//测试5:
//1.不监控任何fd
//2.EV_READ|EV_PERSIST
//3.那只能不断的超时执行回调
void func5()
{
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, -1,  
                                     EV_READ | EV_PERSIST, cmd_cb5, NULL);  
  
    struct timeval tv = {2, 0};
    event_add(cmd_ev, &tv); //超时  
  
    event_base_dispatch(base);  
}

//./16_1.timeout_event 
//in the cmd_cb5
//in the cmd_cb5
//in the cmd_cb5
int main()  
{  
	//func1();
	//func2();
	//func3();
	//func4();
	func5();
    return 0;  
}  
