#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <event3/util.h>
#include <event3/event.h>
#include <event3/thread.h>

#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"


#include<pthread.h> //Linux thread

struct event_base *base = NULL;

void pipe_cb(int fd, short events, void *arg)
{
    printf("in the cmd_cb\n");
}

void timeout_cb(int fd, short events, void *arg)
{
    printf("in the timeout_cb\n");
}

//./15.evthread_make_base_notifiable y
//main event_base_dispatch
//thread_fn wait for input
//a  //输入一个字符让线程把event加入到event_base,并且通知主线程
//in the timeout_cb
//in the timeout_cb
//in the timeout_cb
void* thread_timeout(void *arg)
{
    char ch;
	printf("thread_fn wait for input\n");
    scanf("%c", &ch); //just for wait

    struct event *ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST,
                                 timeout_cb, NULL);

    struct timeval tv = {2, 0};
    event_add(ev, &tv);
}

void sig_cb(int fd, short events, void *arg)  
{  
    printf("in the sig_cb\n");  
}  

//终端1:
//./15.evthread_make_base_notifiable y
//pid = 9079
//main event_base_dispatch
//in the sig_cb
//终端2:
//kill -USR1 9079
void* thread_signal(void *arg)
{
    char ch;
    struct event *ev = evsignal_new(base, SIGUSR1, sig_cb, NULL);  
    event_add(ev, NULL);
}

int main(int argc, char ** argv)
{
    if( argc >= 2 && argv[1][0] == 'y')
        evthread_use_pthreads();

    printf("pid = %d\n", getpid());  
    base = event_base_new();
	//用处:
	//就是防止event_base_dispatch执行时候,
	//子线程还没把event加入event_base造成进程直接退出
    int pipe_fd[2];
    pipe(pipe_fd);
    struct event *ev = event_new(base, pipe_fd[0],
			 EV_READ | EV_PERSIST, pipe_cb, NULL);
    event_add(ev, NULL);

    pthread_t thread;
    //pthread_create(&thread, NULL, thread_timeout, NULL);//使用定时器event
    pthread_create(&thread, NULL, thread_signal, NULL);//使用信号event

	printf("main event_base_dispatch\n");
    event_base_dispatch(base);

    return 0;
}
