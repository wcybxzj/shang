#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


#define FNAME "/etc/services"


//使用O_CLOEXEC,exec后之前打开的文件自动关闭
/*
看到lsof和fuser都没有输出
[root@web11 ~]# lsof /etc/services
[root@web11 ~]# fuser /etc/services
*/
void  cloexec_open()
{
	int ret;
	int fd;

	printf("pid:%d\n", getpid());

	fd = open(FNAME, O_RDONLY|O_CLOEXEC);
	if (fd<0) {
		perror("open():");
		exit(1);
	}

	//1.exec和fork执行前都要fflush 刷新缓冲区
	//如果不刷新原来缓冲区内容就会丢失
	fflush(NULL);

	//2.exec 返回值:
	//成功:整个进程就会替换成里面的命令, 下面的所有语句都不会取执行
	//失败:execl返回-1,所以也不用判断了,只要向下执行就是错了
	execl("/bin/sleep","随便写这里argv[0]", "100", NULL);
	perror("execl():");
	printf("END\n");//因为如果不出错永远执行不到这句
}



//可以看到之前打开的文件并没有在exec后关闭
/*
[root@web11 ~]# lsof /etc/services
COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF    NODE NAME
sleep   6180 root    3r   REG  253,0   641020 2502867 /etc/services
[root@web11 ~]# fuser /etc/services
/etc/services:        6180
*/
void normal_open()
{
	int ret;
	int fd;

	printf("pid:%d\n", getpid());

	fd = open(FNAME, O_RDONLY);
	if (fd<0) {
		perror("open():");
		exit(1);
	}

	//1.exec和fork执行前都要fflush 刷新缓冲区
	//如果不刷新原来缓冲区内容就会丢失
	fflush(NULL);

	//2.exec 返回值:
	//成功:整个进程就会替换成里面的命令, 下面的所有语句都不会取执行
	//失败:execl返回-1,所以也不用判断了,只要向下执行就是错了
	execl("/bin/sleep","随便写这里argv[0]", "100", NULL);
	perror("execl():");
	printf("END\n");//因为如果不出错永远执行不到这句
}


int main(int argc, char *argv[])
{
	//normal_open();
	cloexec_open();
	return 0;
}
