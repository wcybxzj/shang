#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 30
#define FNAME "/etc/services"

void test_sysio_inner_struct1();
void test_sysio_inner_struct2();
void test_sysio_fd_arr();

//http://blog.csdn.net/ordeder/article/details/21716639
//fork父子在系统IO中:
//1.文件描述符数组是各自独立的
//2.内部结构体是共享的
int main(int argc, const char *argv[])
{
	//test_sysio_inner_struct1();
	test_sysio_inner_struct2();
	//test_sysio_fd_arr();
	return 0;
}

/*
父子进程文件表项是共享的，文件表项中有pos文件偏移量,在父子进程都倚靠这个pos来获取数据
现在的情况是父子进程在交替读取同一文件的不同部分
./2.fork_fd 
>>parent pid:6496, buf:# /etc/services:
# $Id: servi<<>>parent pid:6496, buf:1 ovasik Exp $
#
>>child pid:6497, buf:ces,v 1.48 2009/11/11 14:32:3<<>>child pid:6497, bu
f:rvices, Internet style# IANA<<>>child pid:6497, buf:ed 2009-11-10
#
# Network se<<>>parent pid:6496, buf: services version: last updat<<>>par
ent pid:6496, buf:t is presently the policy of <<>>parent pid:6496, buf:known^C
*/
void test_sysio_inner_struct1()
{
	pid_t pid;
	int i, fd;
	char buf[SIZE]={0,};
	fd = open(FNAME, O_RDONLY);
	if (fd<0) {
		perror("open():");
		exit(1);
	}

	pid =  fork();
	if (pid==0) {
		for (i = 0; i < 10; i++) {
			read(fd, buf, SIZE-1);
			printf(">>child pid:%d, buf:%s<<", getpid(), buf);
			//sleep(1);
		}
		close(fd);
		exit(0);
	}

	for (i = 0; i < 10; i++) {
		read(fd, buf, SIZE-1);
		printf(">>parent pid:%d, buf:%s<<", getpid(), buf);
		//sleep(1);
	}
	wait(NULL);
	close(fd);
}

//证明APUE8.3函数fork, page184的内容
//基本和test_sysio_inner_struct1()一样
//父子进程共享文件表项,并且里面有pos,所以在父子在堆同一个fd进行写入的时候也不会出错
void test_sysio_inner_struct2()
{
	pid_t pid;
	int i;

	char *str1 ="parent";
	char *str2 ="child";

	pid =  fork();
	if (pid==0) {
		for (i = 0; i < 10; i++) {
			printf("%s",str2);
			fflush(NULL);
			sleep(1);
		}
		exit(0);
	}

	for (i = 0; i < 10; i++) {
		printf("%s",str1);
		fflush(NULL);
		sleep(1);
	}
	wait(NULL);
}


/*
 * 说明父子进程文件描述符数组是相互独立的
 parent pid:4854, fd:3
 parent pid:4854, fd:4
 parent pid:4854, fd:5
 child pid:4855, fd:3
 child pid:4855, fd:4
 child pid:4855, fd:5
 .child pid:4855, fd:6
 child pid:4855, fd:7
 child pid:4855, fd:8
 child pid:4855, fd:9
 child pid:4855, fd:10
 child pid:4855, fd:11
 child pid:4855, fd:12
 child pid:4855, fd:13
 child pid:4855, fd:14
 child pid:4855, fd:15
 child pid:4855, fd:16
 child pid:4855, fd:17
 parent pid:4854, fd:6
 parent pid:4854, fd:7
 parent pid:4854, fd:8
 */
void test_sysio_fd_arr()
{
	int fd, i=0;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		while (1) {
			fd = open(FNAME, O_RDONLY);
			if (fd < 0) {
				perror("open():");
				exit(1);
			}
			printf("child pid:%d, fd:%d\n", getpid(), fd);
			i++;
			if (i%3==0) {
				sleep(1);
			}
		}
		exit(0);
	}
	while (1) {
		fd = open(FNAME, O_RDONLY);
		if (fd < 0) {
			perror("open():");
			exit(1);
		}
		printf("parent pid:%d, fd:%d\n", getpid(), fd);
		i++;
		if (i%3==0) {
			sleep(5);
		}
	}
	wait(NULL);
}
