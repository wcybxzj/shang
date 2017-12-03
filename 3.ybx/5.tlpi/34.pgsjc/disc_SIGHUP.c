#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int sig)
{
	printf("pid:%d, caught signal:%d,%s \n",
			getpid(), sig, strsignal(sig));
}

/*每个参数都创建一个进程，如果参数是 d 子进程会创建新的进程组*/
/*
echo $$ 当前shell pid
11462

父进程id:11462(exec命令导致的 当前进程替换原来bash 进程号不变)
前台进程组id:11462
进程id:11462, 进程组id:11462(父进程)
进程id:11525, 进程组id:11462(第三个子进程)
进程id:11523, 进程组id:11523(第一个子进程，自己创建组)
进程id:11524, 进程组id:11462(第二个子进程)
pid:11524, caught signal:1,Hangup 
pid:11525, caught signal:1,Hangup 
*/
int main(int argc, const char *argv[])
{
	int i, ret;
	struct sigaction sa;
	pid_t pid;
	if (argc<2 || strcmp(argv[1], "--help")==0) {
		printf("./a.out d s s > /tmp/1.txt\n");
		exit(1);
	}

	printf("父进程id:%d\n",getpid());
	printf("前台进程组id:%d\n",tcgetpgrp(STDIN_FILENO));

	/*重定向到文件时候变成不缓冲*/
	setbuf(stdout, NULL);

	/*创建子进程*/
	for (i = 1; i < argc; i++) {
		pid = fork();
		if (pid==-1) {
			perror("fork()");
			exit(1);
		}
		if (pid==0) {
			if (argv[i][0]=='d') {
				setpgid(0,0);/*创建新进程组并且进入*/
			}

			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = handler;

			ret = sigaction(SIGHUP, &sa, NULL);
			if (ret == -1) {
				perror("sigaction()");
				exit(1);
			}
			break;
		}
	}

	printf("进程id:%d, 进程组id:%d\n",getpid(),getpgid(0));

	/*设置定时器*/
	/*如果60秒 所有进程强制杀死*/
	alarm(60);

	pause();
	return 0;
}
