#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>


static void func(int sig)
{
	printf("pid is:%d,sig is :%d\n", getpid(), sig);
}

//父进程设置的信号处理函数子进程可以继承
//父进程设置的定时器setitimer,子进程不能继承,子进程需要自己去设置setitimer
int main(int argc, const char *argv[])
{
	pid_t pid;
	signal(SIGINT, func);
	signal(SIGUSR1, func);
	signal(SIGALRM, func);

	struct itimerval tm;
	tm.it_interval.tv_sec = 1;
	tm.it_interval.tv_usec = 0;
	tm.it_value.tv_sec = 1;
	tm.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &tm, NULL);

	pid  = fork();
	if (pid<0) {
		printf("fork() error\n");
		exit(1);
	}
	if (pid ==0) {
		printf("child pid:%d\n",getpid());
		//如果想让自己成设置定时器需要在子进程单独设置
		tm.it_interval.tv_sec = 1;
		tm.it_interval.tv_usec = 0;
		tm.it_value.tv_sec = 1;
		tm.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &tm, NULL);
		while (1) {
			sleep(1);
		}
	}else{
		printf("parent id:%d\n",getpid());
		while (1) {
			sleep(1);
		}

	}

	return 0;
}
