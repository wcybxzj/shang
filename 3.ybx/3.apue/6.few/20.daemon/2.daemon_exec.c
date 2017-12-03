#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

#define FNAME "/tmp/out"

#define SIZE 1024

static int daemonize()
{
	pid_t pid;
	int fd;
	pid = fork();
	if (pid < 0) {
		return -1;
	}
	if (pid > 0) {
		exit(0);//父进程退出, 让init收养子进程
		//wait(NULL);
	}

	printf("current child id is %d\n", getpid());

	setsid();

	fd = open("/tmp/123.txt",O_CREAT|O_TRUNC|O_RDWR);
	if (fd < 0) {
		return -2;
	}

	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);

	if (fd > 2) {
		close(fd);
	}

	//假如代码运行在U盘会造成U盘无法卸载
	chdir("/");

	//假如之前限制了文件权限在这全打开
	//文件创建的权限 默认权限&~umask
	umask(0);
	return 0;
}


//证明execl之前对文件操作符的dup或者close,在exec后依然有效
int main(int argc, const char *argv[])
{
	int ret;
	daemonize();//避免和daemon名字冲突

	//ret = execl("./3.exec_programe", "3.exec_programe", NULL);//报错
	ret = execl("/root/www/shangguan/3.ybx/3.apue/6.few/20.daemon/3.exec_programe", "3.exec_programe", NULL);
	if (ret==-1) {
		printf("execl error :%s\n", strerror(errno));
		return -1;
	}

	return 0;
}
