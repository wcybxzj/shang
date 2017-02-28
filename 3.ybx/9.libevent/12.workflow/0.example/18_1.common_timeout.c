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
  
void cmd_cb5(int fd, short events, void *arg)  
{  
	printf("cb5!!!!!!!!!\n");
}  

void cmd_cb6(int fd, short events, void *arg)  
{  
	printf("cb6!!!!!!!!!\n");
}  

void func5()
{
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, -1,  
                                     EV_READ | EV_PERSIST, cmd_cb5, NULL);  
  
    struct event *cmd_ev1 = event_new(base, -1,  
                                     EV_READ | EV_PERSIST, cmd_cb6, NULL);  

    struct timeval tv = {2, 0};

    struct timeval *tvp=NULL;
    struct timeval *tvp1=NULL;
	tvp = event_base_init_common_timeout(base, &tv);
	if (tvp==NULL) {
		perror("tvp not have!");
		exit(1);
	}
	tvp1 = event_base_init_common_timeout(base, &tv);
	if (tvp1==NULL) {
		perror("tvp not have!");
		exit(1);
	}
    event_add(cmd_ev, tvp); //超时  
    event_add(cmd_ev1, tvp1); //超时  
  
    event_base_dispatch(base);  
}

//测试:通用超时队列和最小堆配合使用
int main()  
{  
	func5();
    return 0;  
}  
