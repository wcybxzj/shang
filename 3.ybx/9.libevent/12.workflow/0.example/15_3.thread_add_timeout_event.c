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

void* thread_timeout(void *arg)
{
    char ch;
	printf("thread_timeout wait for input\n");
    scanf("%c", &ch); //用处是保证main thread中的event_base_dispatch先运行起来
	//1.这里的event之所以表现成定时效果,每2秒执行一次是因为event没关联任何fd
	//2.EV_TIMEOUT:
	//这个标志表示某超时时间流逝后事件成为激活的。
	//构造事件的时候，EV_TIMEOUT标志是被忽略的：
	//可以在添加事件的时候设置超时，也可以不设置。
	//超时发生时，回调函数的what参数将带有这个标志。
    struct event *ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST,
                                 timeout_cb, NULL);
    struct timeval tv = {2, 0};
    event_add(ev, &tv);
	printf("定时器event address:%x\n", ev);
}

void* thread_timeout1(void *arg)
{
    struct event *ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST,
                                 timeout_cb, NULL);
    struct timeval tv = {2, 0};
    event_add(ev, &tv);
}

void sig_cb(int fd, short events, void *arg)  
{  
    printf("in the sig_cb\n");  
}  

//event_base_dispatch先于分线程添加的定时器event运行才需要libevent的多线程机制
//
//测试3:
//如果不开启多线程, 弟线程向event_base注册定时器event 不会去通知给main线程
//./15_1.evthread_make_base_notifiable_different_event 
//pid = 6806
//thread_timeout wait for input
//main event_base_dispatch
//a

//测试4(重点,用这个例子调试笔记中的效果):
//如果开启多线程, 弟线程向event_base注册定时器event 会去通知给main线程,并且event定时执行
//./15_1.evthread_make_base_notifiable_different_event y
//pid = 6818
//main event_base_dispatch
//thread_timeout wait for input
//a
//in the timeout_cb
//in the timeout_cb
void test_timeout()
{
	base = event_base_new();

	//event_base_dispatch运行时候必须在eventquque或者activequeues存在event
	int pipe_fd[2];
	pipe(pipe_fd);
	struct event *ev = event_new(base, pipe_fd[0],
			EV_READ | EV_PERSIST, pipe_cb, NULL);
	event_add(ev, NULL);
	printf("普通io event address:%x\n", ev);
	pthread_t thread;
	pthread_create(&thread, NULL, thread_timeout, NULL);//使用定时器event

	printf("main event_base_dispatch\n");
	event_base_dispatch(base);
}


//event_base_dispatch 后于分线程添加的定时器event运行,无需开启libevent多线程
//
//测试5:
//./15_1.evthread_make_base_notifiable_different_event 
//pid = 9013
//main event_base_dispatch
//in the timeout_cb
//in the timeout_cb
//
//测试6:
//./15_1.evthread_make_base_notifiable_different_event y
//pid = 9015
//main event_base_dispatch
//in the timeout_cb
//in the timeout_cb
void test_timeout1()
{
	printf("pid = %d\n", getpid());  
	base = event_base_new();

	int pipe_fd[2];
	pipe(pipe_fd);
	struct event *ev = event_new(base, pipe_fd[0],
			EV_READ | EV_PERSIST, pipe_cb, NULL);
	event_add(ev, NULL);

	pthread_t thread;
	pthread_create(&thread, NULL, thread_timeout1, NULL);//使用定时器event

	sleep(1);
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

	test_timeout();
	//test_timeout1();

	return 0;
}
