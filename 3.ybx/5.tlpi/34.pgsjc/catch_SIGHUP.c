#if ! defined(_XOPEN_SOURCE) || _XOPEN_SOURCE < 500
#define _XOPEN_SOURCE 500
#endif
#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void handler(int s)
{
	//printf("进程id:%d:catch %s\n", getpid(), strsignal(s));
}

/*
实验1:
父子进程都是9362 都是bash创建的进程组，
终端关闭,bash收到SIGHUP,bash会将SIGHUP发给所有自己创建的进程组(前台和后台进程组)

echo $$ (当前shell的pid)
8025

./catch_SIGHUP > /tmp/1.txt
然后关闭终端

cat /tmp/1.txt
进程id=9362, 父进程id=8025, 进程组id=9362, 会话id=8025
进程id=9363, 父进程id=9362, 进程组id=9362, 会话id=8025
进程id:9363:catch SIGHUP
进程id:9362:catch SIGHUP
*/

/*
实验2:
父进程的进程组是bash创建
子进程的进程组是自己创建
终端关闭,bash收到SIGHUP,bash会将SIGHUP发给所有自己创建的进程组(前台和后台进程组)
可以看到子进程就没收到信号

echo $$
9030

./catch_SIGHUP a > /tmp/2.txt
然后关闭终端

cat /tmp/2.txt
进程id=9376, 父进程id=9030, 进程组id=9376, 会话id=9030
进程id=9377, 父进程id=9376, 进程组id=9377, 会话id=9030
进程id:9376:catch SIGHUP
*/
int main(int argc, const char *argv[])
{
	int ret;
	pid_t pid;
	struct sigaction sa;

	/*必须将stdout变成不缓冲 才能在重定向到文件的时候写进去内容*/
	setbuf(stdout, NULL);

	/*父子进程都设置信号处理*/
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	ret = sigaction(SIGHUP, &sa, NULL);
	if (ret == -1) {
		perror("sigaction()");
		exit(1);
	}

	pid = fork();
	if (pid<0) {
		perror("fork()");
		exit(1);
	}

	/*子进程创建一个新进程组，并且进入新进程组*/
	if (pid==0 && argc>1) {
		ret =setpgid(0,0);
		if (ret == -1) {
			perror("setpgid()");
			exit(1);
		}
	}

	printf("进程id=%d, 父进程id=%d, 进程组id=%d, 会话id=%d \n",
			getpid(), getppid(), getpgid(0), getsid(0));

	/*60秒后定时信号保证程序会被杀死*/
	alarm(60);

	//等信号发生
	pause();
	printf("进程id:%d:catch SIGHUP\n", getpid());

	return 0;
}
