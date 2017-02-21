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
  
  
int main()  
{  
    evthread_use_pthreads();  
  
    //使用默认的event_base配置  
    struct event_base *base = event_base_new();  
  
    struct event *cmd_ev = event_new(base, STDIN_FILENO,  
                                     EV_READ | EV_PERSIST, cmd_cb, NULL);  
  
    event_add(cmd_ev, NULL); //没有超时  
  
    event_base_dispatch(base);  
  
    return 0;  
}  
