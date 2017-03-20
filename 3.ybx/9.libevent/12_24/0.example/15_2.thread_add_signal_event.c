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


#include <pthread.h> //Linux thread

struct event_base *base = NULL;

void pipe_cb(int fd, short events, void *arg)
{
    printf("in the cmd_cb\n");
}

void timeout_cb(int fd, short events, void *arg)
{
    printf("in the timeout_cb\n");
}


void sig_cb(int fd, short events, void *arg)  
{  
    printf("in the sig_cb\n");  
}  

void* thread_signal(void *arg)
{
    char ch;
	printf("thread_signal wait for input\n");
    scanf("%c", &ch); //用处是保证main thread中的event_base_dispatch先运行起来
    struct event *ev = evsignal_new(base, SIGUSR1, sig_cb, NULL);  
    event_add(ev, NULL);
	//printf("信号event address:%x\n", ev);
}

//无论是否开启多线程, 弟线程向event_base注册信号event 都会去通知给main线程,信号到来就会执行event对应的回调
//
//测试1:
//终端1:
//./15_1.evthread_make_base_notifiable_different_event 
//pid = 8027
//main event_base_dispatch
//thread_signal wait for input
//a
//in the sig_cb
//终端2:
//kill -USR1 8027

//测试2:
//终端1:
//./15_1.evthread_make_base_notifiable_different_event y
//pid = 8030
//main event_base_dispatch
//thread_signal wait for input
//a
//in the sig_cb
//终端2:
//kill -USR1 8030
void test_sginal()
{
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
	pthread_create(&thread, NULL, thread_signal, NULL);//使用信号event

	printf("main event_base_dispatch\n");

	event_base_dispatch(base);
}

//libevent的多线程会开启通知机制

//分线程添加信号event:
//测试1和测试2:
//弟线程添加的信号event无论libevent是否开启多线都生效

//分线程添加定时器event:
//测试3和测试4:
//event_base_dispatch先于分线程添加的定时器event运行才需要libevent的多线程机制
//测试5和测试6:
//event_base_dispatch 后于分线程添加的定时器event运行,无需开启libevent多线程

//测试4就是查看libevent多线程通知机制的入口和重点
int main(int argc, char ** argv)
{
	if( argc >= 2 && argv[1][0] == 'y')
		evthread_use_pthreads();

	test_sginal();
	return 0;
}
