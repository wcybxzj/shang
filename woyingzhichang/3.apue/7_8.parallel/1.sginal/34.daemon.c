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

	fd = open("/dev/null", O_RDWR);
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


int main(int argc, const char *argv[])
{
	FILE *fp;
	int i, ret;
	char str[SIZE];
	openlog("YBX_DAEMON", LOG_PID|LOG_PERROR, LOG_DAEMON);

	ret = daemonize();//避免和daemon名字冲突

	if (ret) {
		syslog(LOG_ERR, "daemonize failed!");//注意别写换行
		exit(1);
	}else{
		syslog(LOG_INFO, "daemonize() success");
	}

	fp = fopen(FNAME, "w");
	if (NULL == fp) {
		syslog(LOG_ERR, "fopen:%s", strerror(errno));
		exit(1);
	}

	syslog(LOG_INFO, "%s is opened!", FNAME);

	for (i = 0; ; i++) {
		snprintf(str, SIZE, "%d\n", i);
		fputs(str, fp);
		fflush(fp);
		syslog(LOG_DEBUG, "%d is printed!", i);
		sleep(1);
	}

	//永远执行不了这两句，守护进程脱离控制住终端, kill -9 进程号,信号来了异常被终止
	closelog();
	fclose(fp);
	return 0;
}
