#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* Simple error handling functions */
#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

	static void *
sig_thread(void *arg)
{
	sigset_t *set = (sigset_t *) arg;
	int s, sig;

	for (;;) {
		s = sigwait(set, &sig);
		if (s != 0)
			handle_error_en(s, "sigwait");
		printf("Signal handling thread got signal %d\n", sig);
	}
}

//man pthread_sigmask
//主线程不处理阻塞信号,不对信号进程处理
//让子线程去处理信号SIGUSR1和SIGQUIT
//
//测试方法:
//[root@web11 14.pthread]# ./pthread_sigmask 
//pid:13467
//Signal handling thread got signal 10
//Signal handling thread got signal 3
//
//[root@web11 7.sginal]# kill -USR1 13467
//[root@web11 7.sginal]# kill -QUIT 13467
//
int main(int argc, char *argv[])
{
	pthread_t thread;
	sigset_t set;
	int s;

	printf("pid:%d\n", getpid());

	/* Block SIGINT; other threads created by main() will inherit
	   a copy of the signal mask. */

	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGUSR1);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_sigmask");

	s = pthread_create(&thread, NULL, &sig_thread, (void *) &set);
	if (s != 0)
		handle_error_en(s, "pthread_create");

	/* Main thread carries on to create other threads and/or do
	   other work */

	pause();            /* Dummy pause so we can test program */
}
