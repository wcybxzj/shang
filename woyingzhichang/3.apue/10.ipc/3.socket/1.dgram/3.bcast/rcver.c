#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto.h"

#define SIZE 16

int main(){
	int sd;
	struct msg_st *rbuf = NULL;
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

	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST, \
				&val, sizeof(val)) < 0){
		perror("setscokopt()");
		exit(-1);
	}

	if(bind(sd, (void *)&laddr, sizeof(laddr))){
		perror("bind()");
		exit(-1);
	}

	rbuf = malloc(MSGMAX);
	if (rbuf == NULL) {
		perror("malloc()");
		exit(0);
	}

	raddr_len = sizeof(raddr);
    while(recvfrom(sd, rbuf, MSGMAX, 0, (void *)&raddr, &raddr_len) > 0){
		if(inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ip, SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}
		printf("from ip:%s port:%d\n",ip, ntohs(raddr.sin_port));
		printf("%s\n", rbuf->name);
		printf("%d\n", ntohl(rbuf->math));
		printf("%d\n", ntohl(rbuf->chinese));
	}

	close(sd);
	exit(0);
}
