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

#include "proto.h"
#define IP_SIZE 16

int work = 1;
static void func(int s){
	work = 0;
}

//目的:查看listen中backlog的作用
//netstat -antp|grep 1989
//listen backlog设置成1, ESTABLISH的最大数量是backlog+1=2
//4个client 来请求2个成为SYN_RECV 2个是ESTABLISH,
//但最后都变成ESTABLISH并且正常响应了
int worker(int newsd){
	char str[IP_SIZE]={'\0'};
	int len;

	len = sprintf(str, FMT_STAMP, (long long)time(NULL))+1;

	printf("=====len:%d======\n", len);
	printf(FMT_STAMP, str);
	if(send(newsd, str, len, 0) < 0){
		perror("send()");
		exit(-3);
	}
}


int main(){
	int num=0;
	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	signal(SIGINT, func);

	sd = socket(AF_INET, SOCK_STREAM, 0);
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

	listen(sd, 1);
	printf("sever sleep\n");
	while (work) {
		sleep(1);
		printf("num:%d\n",num);
		num++;
	}
	printf("sever sleep done\n");
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
