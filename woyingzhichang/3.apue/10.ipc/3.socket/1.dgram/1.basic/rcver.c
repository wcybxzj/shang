#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto.h"

//strlen("255.255.255.255")+字符串'\0'
//15+1=16
#define SIZE 16

int main(){
	int sd;
	struct msg_st rbuf;
	struct sockaddr_in laddr, raddr;
	socklen_t raddr_len;
	char ip[SIZE];

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0){
		perror("socket()");
		exit(-1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr) != 1){
		perror("inent_pton()");
		exit(-1);
	}
	if(bind(sd, (void *)&laddr, sizeof(laddr))){
		perror("bind()");
		exit(-1);
	}

	raddr_len = sizeof(raddr);//重点
    while(recvfrom(sd, &rbuf, sizeof(rbuf), 0, (void *)&raddr, &raddr_len) > 0){
		if(inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ip, SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}
		printf("from ip:%s port:%d\n",ip, ntohs(raddr.sin_port));
		printf("%s\n", rbuf.name);
		printf("%d\n", ntohl(rbuf.math));
		printf("%d\n", ntohl(rbuf.chinese));
	}

	sleep(10);
	close(sd);
	exit(0);
}
