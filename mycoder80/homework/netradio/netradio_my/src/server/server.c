#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>

#include <proto.h>

#include "server_conf.h"
/*
 *  -M      指定多播组
 *  -P      指定收接端口
 *  -C		指定测试频道
 *  -F		前台调试运行
 *  -D		媒体库位置
 *  -I		设置网卡
 *  -H      显示帮助
 * */

struct server_conf_st server_conf{
	.rcvport = DEFAULT_RCVPORT,
	.mgroup = DEFAULT_MGROUP,
	.media_dir = DEFAULT_MEDIADIR,
	.run_mode = RUN_DAEMON,
	.ifname = DEFAULT_IF
};

static void print_help(void){
	printf("-M 指定多播组\n
			-P 指定收接端口\n
			-C 指定测试频道\n
			-F 前台调试运行\n
			-D 媒体库位置\n
			-I 设置网卡\n
			-H 显示帮助\n");
}

static void daemon_exit(int s)
{
	closelog();
	exit(0);
}

static int  daemonize(){
	int fd;
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		//perror("fork()");
		return -1;
		syslog(LOG_ERR, "fork(): %s", strerror(errno));
	}

	if (pid > 0) {
		exit(0);
	}
	//还没脱离终端
	fp = open("/dev/null", ORDWR);
	if (fd < 0) {
		//perror("open()");
		syslog(LOG_WARNING, "open():%s", strerror(errno));
		return -2;
	}

	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);

	if (fd > 2) {
		close(fd);
	}

	setsid();

	//防止进程运行在设备上
	//造成设备无法卸载
	//device is busy
	chdir("/");
	umask(0);
	return 0;
}

static void socket_init(void)
{
	int serversd;
	struct ip_mreqn mreq;
	serversd = socket(AF_INET,SOCK_DGRAM,0);
	if (serversd<0) {
		syslog(LOG_ERR, "sockert():%s", strerror(errno));
		exit(1);
	}

	inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(server_conf.ifname);
	if (setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0) {
		syslog(LOG_ERR, "setsockopt(IP_MULTICAST_IF):%s", strerror(errno));
		exit(0);
	}
}


int main(int argc, char *argv[])
{
	int c;

	sa.sa_handler = daemon_exit;
	sigemptyset(sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGQUIT);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaction(SIGTERM, sa, NULL);
	sigaction(SIGINT, sa, NULL);
	sigaction(SIGQUIT, sa, NULL);
	sa.sa_flags = 0;

	openlog("netradio", LOG_PID|LOG_PERROR, LOG_DAEMON);

	/*命令行分析*/
	while (1) {
		c = getopt(argc, argv,"M:P:C:FD:I:H");
		switch(c){
			case 'M':
				server_conf.mgroup = optarg;
				break;
			case 'P':
				server_conf.rcvport= optarg;
				break;
			case 'C':
				break;
			case 'F':
				server_conf.run_mode= RUN_FOREGROUND;
				break;
			case 'D':
				server_conf.media_dir = optarg;
				break;
			case 'I':
				server_conf.ifname = optarg;
				break;
			case 'H':
				print_help();
				exit(0);
				break;
			default:
				abort();//终止当前进程并且获得执行现场
				break;
		}
	}

	/*守护进程*/
	if (server_conf.run_mode == RUN_DAEMON) {
		if (daemonize() != 0) {
			exit(1);
		}
	}else if(server_conf.run_mode == RUN_FOREGROUND)
		//nothing
	}else{
		//fprintf(stderr, "EINVAL\n");//参数非法
		syslog(LOG_ERR, "EINVAL server_conf.runmode");//参数非法
		exit(1);
	}

	/*socket init*/
	socket_init();


	/*获取频道列表信息（从medialib中）*/
	struct mlib_listentry_st *list;
	int list_size;
	int err;
	err = mlib_getchnlist(&list, &list_size);
	if () {

	}

	/*创建thr_list*/
	thr_list_create(list, list_size);
	/**if error*/


	/*创建thr_channel*/
	/*1:200  100:200  4:200  200:200*/
	int i;
	for (i = 0; i < list_size; i++) {
		thr_channel_create(list+i);
		/*if error*/
	}

	syslog(LOG_DEBUG, "%d channel threads created." ,i);

	while(1)
		pause();

	//closelog();

	exit(0);
}
