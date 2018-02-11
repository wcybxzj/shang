#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "proto.h"
#define SIZE 11
void func1(int sd){
	char str[SIZE]={'\0'};
	read(sd, str, SIZE);
	printf("%s\n", str);
	close(sd);
}

void func2(int sd){
	FILE *fp;
	long long time;
	fp = fdopen(sd, "r+");
	if(fp == NULL){
		perror("fdopen()");
		exit(-2);
	}

	if(fscanf(fp, FMT_STAMP, &time) <0){
		perror("fscanf()");
		exit(-2);
	}else{
		printf("%lld\n", time);
	}
	fclose(fp);
	close(sd);
}

//最简单测试server的方法
//nc 127.0.0.1 1998
//telent 127.0.0.1 1998
int main(int argc, const char *argv[])
{
	int sd;
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
		printf("err:%s\n",strerror(errno));
		exit(-2);
	}

	//func1(sd);
	func2(sd);

	return 0;
}
