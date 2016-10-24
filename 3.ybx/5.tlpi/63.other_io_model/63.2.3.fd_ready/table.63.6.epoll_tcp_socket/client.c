#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <limits.h>
#include "proto.h"
#include <sys/epoll.h>
#include <signal.h>

#define SIZE 11

#define BUF_SIZE 1024
int sd;

void sig_handler(int s){
	printf("sig_hanler\n");
	close(sd);
	//shutdown(sd, SHUT_WR);
}

void func2(){
	int i;
	int ret;
	char str[BUF_SIZE];
	printf("sleep 10\n");
	sleep(10);
	printf("write abc \n");
	write(sd,"abc",3);
	pause();
}

int main(int argc, const char *argv[])
{
	struct sockaddr_in saddr;
	if(argc!=2){
		perror("lack argc");
		exit(-1);
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("socket");
		exit(-2);
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(SERVERPORT));
	if(inet_pton(AF_INET, argv[1], &saddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(connect(sd, (void *)&saddr, sizeof(saddr)) < 0){
		perror("connect()");
		exit(-2);
	}

	signal(SIGINT, sig_handler);

	func2(sd);

	return 0;
}
