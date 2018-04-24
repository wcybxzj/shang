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

int main(int argc, char *argv[]){
	if (argc != 4) {
		printf("./a.out ip  port is_SO_REUSEADDR\n");
		exit(1);
	}
	int use;
	int num=0;
	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	int port = atoi(argv[2]);
	if (strcmp(argv[3],"use")==0) {
		printf("use SO_REUSEADDR\n");
		use=1;
	}else{
		printf("not use SO_REUSEADDR\n");
		use=0;
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
		perror("socket");
		exit(0);
	}

	if (use) {
		int val=1;
		if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,\
					&val, sizeof(val)) < 0){
			perror("setsockopt()");
			exit(0);
		}
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(port);
	if(inet_pton(AF_INET,argv[1],&laddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(0);
	}

	listen(sd, 10);

	while (1) {
		newsd = accept(sd, (void *)&raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		worker(newsd);

	}

	exit(0);
}
