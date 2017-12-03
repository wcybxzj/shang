#include "tlpi_hdr.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

//证明可以 打开/dev/tty
static void test1()
{
	int fd = open("/dev/tty",O_RDWR);
	if (fd<0) {
		printf("error\n");
	}else{
		printf("ok\n");
	}
}


//书上本代码的原始例子
//fork后 父进程退出，子进程setsid 然后 打开/dev/tty报错
//因为setsid()后子进程脱离控制终端，而/dev/tty表示当前终端，因为不存在所以报错
static void test2()
{
	int ret;
	pid_t pid;
	pid = fork();
	if (pid<0) {
		perror("fork() error");
		exit(1);
	}

	if (pid>0) {
		exit(0);
	}

	ret = setsid();
	if (ret <0) {
		perror("setsid() error");
		exit(1);
	}

	printf("pid:%ld pgid:%ld, sid:%ld\n",
			(long)getpid(), (long)getpgrp(), (long)getsid(0));

	//ret = open("/dev/tty",O_RDWR);//报错
	if (ret==-1) {
		perror("open() error");
		exit(1);
	}
}
/*
fork后 父进程退出，子进程setsid
第一点:
open(/dev/tty10) 后再去open(/dev/tty) 没报错
因为当前进程是session leader 打开/dev/tty10后,/dev/tty10成为控制终端
第二点：
/dev/tty在这里就是/dev/tty10, /dev/tty相当于一个变量,表示当前进控制终端是这个指向的那个终端
写/dev/tty就是写/dev/tty10
*/
static void test3()
{
	int i, ret, fd1, fd2;

	pid_t pid;
	pid = fork();
	if (pid<0) {
		perror("fork() error");
		exit(1);
	}

	if (pid>0) {
		exit(0);
	}

	ret = setsid();
	if (ret <0) {
		perror("setsid() error");
		exit(1);
	}

	printf("pid:%ld pgid:%ld, sid:%ld\n",
			(long)getpid(), (long)getpgrp(), (long)getsid(0));

	fd1 = open("/dev/tty10",O_RDWR);
	if (fd1 ==-1) {
		perror("open() error");
		exit(1);
	}else{
		printf("fd1:%d\n",fd1);
	}

	fd2 = open("/dev/tty",O_RDWR);
	if (fd2 ==-1) {
		perror("open() error");
		exit(1);
	}else{
		printf("fd2:%d\n",fd2);
	}

	//获取当前控制终端的名称 tlpi34.4 page580
	char buf[100];
	ctermid(buf);
	printf("当前控制终端的名称为:%s\n",buf);//输出 /dev/null

	char *str ="fd2 write";
	for (i = 0; i < 10; i++) {
		sleep(1);
		write(fd2, str, strlen(str));
	}
}


//tty命令获取当前虚拟终端
//$$:获取当前进程
/*
[root@web11 34.pgsjc]# tty
/dev/pts/2
[root@web11 34.pgsjc]# echo $$
12303
*/
int main(int argc, const char *argv[])
{
	//test1();
	//test2();
	test3();
	return 0;
}
