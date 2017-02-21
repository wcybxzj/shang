#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <event.h>
#include <string.h>
#include <errno.h>                                                                                                
#include <event2/thread.h>


void cmd_cb(int fd, short event, void *arg)  
{  
  
}  

//简单演示
//一旦设置了线程、锁函数，那么就不应该对其进行修改
int main()  
{  
    evthread_use_pthreads();  
  
    struct event_base *base = event_base_new();  
  
    evthread_set_lock_callbacks(NULL);  
  
    struct event *cmd_event = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST,  
			                                 cmd_cb, base);  
    event_add(cmd_event, NULL);  
  
    event_base_dispatch(base);  
  
    return 0;  
}  
