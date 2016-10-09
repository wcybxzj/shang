#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <signal.h>

#define FNAME "/tmp/out"

#define SIZE 1024
FILE *fp;
//用法:kill -2 pid 后有时间kill -3 pid　
//signal_deal();存在重入问题无法释放资源,让进程正常结束
//sigaction_deal();让不同信号对同以函数的处理串行解决函数重入中的问题
//tail -f /var/log/syslog 观察

static void daemon_exit(int s)
{
	syslog(LOG_INFO, "daemon_exit signal is %d", s);
	fclose(fp);//不允许执行两次 35.double_fclose.c
	sleep(5);
	syslog(LOG_INFO, "process normal exit!");
	closelog();
	exit(0);
}

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

//signal vs sigaction 二选一
void signal_deal()
{
	signal(SIGINT, daemon_exit);
	signal(SIGTERM, daemon_exit);
	signal(SIGQUIT, daemon_exit);
}

//signal vs sigaction 二选一
void sigaction_deal()
{
	struct sigaction sa;
	/*
	写法1:标准写法
	SIGINT的处理中另外两个信号mask也设置成0,避免多个信号对同一个处理函数的重入
	*/
	//sa.sa_handler = daemon_exit;
	//sigemptyset(&sa.sa_mask);
	//sigaddset(&sa.sa_mask, SIGQUIT);
	//sigaddset(&sa.sa_mask, SIGTERM);
	//sa.sa_flags = 0;
	//sigaction(SIGINT, &sa, NULL);
	//
	//sa.sa_handler = daemon_exit;
	//sigemptyset(&sa.sa_mask);
	//sigaddset(&sa.sa_mask, SIGINT);
	//sigaddset(&sa.sa_mask, SIGQUIT);
	//sa.sa_flags = 0;
	//sigaction(SIGQUIT, &sa, NULL);
	//
	//sa.sa_handler = daemon_exit;
	//sigemptyset(&sa.sa_mask);
	//sigaddset(&sa.sa_mask, SIGINT);
	//sigaddset(&sa.sa_mask, SIGTERM);
	//sa.sa_flags = 0;
	//sigaction(SIGTERM, &sa, NULL);

	/*
	写法2:更精简的写法:
	SIGINT为例:
	与标准写法比处理SIGINT函数中多BLOCK了一次SIGINT
	因为就在处理信号时候本身就要mask从1设置成0,所以多再设置一次mask 0也没事
	*/
	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGQUIT);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

int main(int argc, const char *argv[])
{
	int i, ret;
	char str[SIZE];
	openlog("YBX_DAEMON", LOG_PID|LOG_PERROR, LOG_DAEMON);

	//二选一signal和sigaction
	//signal_deal();
	sigaction_deal();

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

	////永远执行不了这两句，守护进程脱离控制住终端, kill -9 进程号,信号来了异常被终止
	//closelog();
	//fclose(fp);
	return 0;
}
