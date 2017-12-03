#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


static void func(int sig)
{
	printf("pid is:%d,sig is :%d\n", getpid(), sig);
}
//证明父进程的信号设置在进程一样会继承
/*
测试方法1:
[root@web11 1.star]# kill -INT 9236
[root@web11 1.star]# kill -INT 9237
[root@web11 1.fork]# ./4.fork_signal
parent id:9899
child pid:9900
pid is:9899,sig is :2
pid is:9900,sig is :2
*/

/*
 测试方法2:
 [root@web11 1.fork]# ./4.fork_signal
 parent id:9924
 child pid:9925
 ^Cpid is:9924,sig is :2
 pid is:9925,sig is :2

 */
int main(int argc, const char *argv[])
{
	pid_t pid;
	signal(SIGINT, func);
	pid  = fork();
	if (pid<0) {
		printf("fork() error\n");
		exit(1);
	}
	if (pid ==0) {
		printf("child pid:%d\n",getpid());
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
