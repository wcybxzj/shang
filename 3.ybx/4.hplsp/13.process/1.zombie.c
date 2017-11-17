#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

//第一种僵尸情况:子进程结束父进行活着但是不回收子进程
/*
[root@web11 6.few]# ps axf|grep zombie
  5331 pts/0    S+     0:00  |       \_ ./1.zombie
    5332 pts/0    Z+     0:00  |           \_ [1.zombie] <defunct>
*/
int main(int argc, const char *argv[])
{
	pid_t pid;

	pid = fork();
	if (pid==0) {
		printf("child pid:%d\n", getpid());
		exit(1);
	}

	printf("parent pid:%d\n", getpid());
	sleep(10000);
	wait(NULL);
	return 0;
}
