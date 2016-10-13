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
#include <poll.h>
#include <limits.h>
#include <sys/socket.h>

#include "proto.h"

#define IP_SIZE 16


int worker(int newsd){
	int i, len, ret;
	char str[IP_SIZE]={'\0'};
	struct pollfd pollfd_var;

	//poll 可输出 POLLOUT
	pollfd_var.fd = newsd;
	pollfd_var.events = POLLIN|POLLOUT|POLLPRI;
	printf("block\n");
	ret = poll(&pollfd_var, 1, -1);
	if (ret == -1) {
		perror("poll");
		exit(1);
	}
	if (pollfd_var.revents & POLLERR) {
		printf("POLLERR\n");
	}
	if(pollfd_var.revents & POLLOUT){
		printf("POLLOUT\n");
	}
	if(pollfd_var.revents & POLLIN){
		printf("POLLIN\n");
	}

	sleep(2);
	//send
	len = sprintf(str, FMT_STAMP, (long long)time(NULL))+1;
	if(send(newsd, str, len, 0) < 0){
		perror("send()");
		exit(-3);
	}
	
	sleep(100);
	printf("close or shutdown\n");
	shutdown(newsd, SHUT_WR);
	//close(newsd);
}


//表63-6
int main(){
	int i, sd, newsd, ret;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];
	struct pollfd pollfd_var;

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

	//listen不存在阻塞的问题,要么立刻成功,要么立刻失败
	listen(sd, 200);

	//client connect后,server accept前
	//poll 返回POLLIN
	printf("poll\n");
	pollfd_var.fd = sd;
	pollfd_var.events = POLLIN|POLLOUT|POLLPRI;
	ret = poll(&pollfd_var, 1, -1);
	if (ret == -1) {
		perror("poll");
		exit(1);
	}
	if (pollfd_var.revents & POLLERR) {
		printf("POLLERR\n");
	}
	if(pollfd_var.revents & POLLOUT){
		printf("POLLOUT\n");
	}
	if(pollfd_var.revents & POLLIN){
		printf("POLLIN\n");
	}
	
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

		//printf("radd:%s rport:%d\n", \
		//		ip, htons(raddr.sin_port));

		worker(newsd);
	}

	exit(0);
}
