#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define FNAME "/etc/services"

void test()
{
	pid_t pid;
	int i, fd;
	fd = open(FNAME, O_RDONLY);
	if (fd<0) {
		perror("open():");
		exit(1);
	}
	printf("父进程:%d\n", getpid());
	pid =  fork();
		printf("子进程:%d\n", getpid());
	if (pid==0) {
		sleep(1000);
		exit(0);
	}
	sleep(1000);
	wait(NULL);
	close(fd);
}
/*
[root@web11 ~]# lsof -p 5305
COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF    NODE NAME
3.fork  5305 root    3r   REG  253,0   641020 2502867 /etc/services

[root@web11 ~]# lsof -p 5306
COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF    NODE NAME
3.fork  5306 root    3r   REG  253,0   641020 2502867 /etc/services

[root@web11 ~]# fuser /etc/services
/etc/services:        5305  5306
 */

//父先打开文件 然后fork
//lsof和fuser查看文件引用计数是 2个进程都在
int main(int argc, const char *argv[])
{
	test();
	return 0;
}

