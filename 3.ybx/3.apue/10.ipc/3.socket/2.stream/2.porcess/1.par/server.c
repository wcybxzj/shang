#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "proto.h"
#define IP_SIZE 16

int func2(int newsd){
	char str[IP_SIZE]={'\0'};
	int len;

	//如果这样写fork后的父进程不用close(newsd);
	//len = sprintf(str, FMT_STAMP, (long long)time(NULL))+1;
	len = sprintf(str, FMT_STAMP, (long long)time(NULL));

	printf("===pid===len:%d======\n", getpid(), len);
	//printf(FMT_STAMP, str);
	if(send(newsd, str, len, 0) < 0){
		perror("send()");
		exit(-3);
	}
	//printf("子进程 already send\n");
	close(newsd);//如果不close fd会泄露
	//printf("子进程结束\n");
}

//测试看到100个请求，server产生100个子进程服务
//for i in {1..100}; do { ./client 127.0.0.1; }& done
int main(){
	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];
	pid_t pid;

	sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);
	if(sd < 0){
		perror("socket");
		exit(0);
	}

	int val=1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){
		perror("setsockopt()");
		exit(0);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	if(inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(0);
	}

	listen(sd, 200);
	rlen = sizeof(raddr);
	while (1) {
		newsd = accept(sd, (void *)&raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		pid = fork();
		if(pid == 0){
			if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
						ip, IP_SIZE) == NULL){
				perror("inet_ntop()");
				exit(-2);
			}

			printf("radd:%s rport:%d\n", \
					ip, htons(raddr.sin_port));
			close(sd);//关闭从父进程继承的不需要使用的sd
			func2(newsd);
			exit(0);
		}else if(pid<0){
			perror("fork()");
			exit(-2);
		}else{
			//重点，这里如果不关闭newsd客户端得不到输出
			//关闭用于子进程的newsd,防止泄露,导致最后父进程sd 数量超过ulimit -a
			close(newsd);
		}
	}

	exit(0);
}
