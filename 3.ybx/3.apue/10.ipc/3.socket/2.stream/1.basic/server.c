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

int worker(int newsd){
	char str[IP_SIZE]={'\0'};
	int len;
	int ret;


	//问题:
	//client就不会输出

	//办法1: len+1
	//这里len加1的用处2点:
	//1:给字符最后加个为零否则客户端输出的字符有问题
	//2:解决server sleep中, client 无法输出的问题
	//len = sprintf(str, FMT_STAMP, (long long)time(NULL))+1;
	len = sprintf(str, FMT_STAMP, (long long)time(NULL));

	printf("=====len:%d======\n", len);
	printf(FMT_STAMP, str);

	ret = send(newsd, str, len, 0);

	if(ret < 0){
		perror("send()");
		exit(-3);
	}else{
		printf("send ok");
	}


	//办法2:
	close(newsd);
}


int main(){
	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);
	if(sd < 0){
		perror("socket");
		exit(0);
	}

	//问题:
	//server启动并且服务1个client
	//server close(newfd) 在tcp主动关闭将残留TIME_WAIT持续2MSL
	//关闭server 再次启动server
	//报错bind(): Address already in use

	//方法:
	//通过设置SO_REUSEADDR, 让server再次启动bind时无视残留的TIME_WAIT
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
	while (1) {
		newsd = accept(sd, (void *)&raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}

		printf("radd:%s rport:%d\n", \
				ip, htons(raddr.sin_port));

		worker(newsd);
	}

	exit(0);
}
