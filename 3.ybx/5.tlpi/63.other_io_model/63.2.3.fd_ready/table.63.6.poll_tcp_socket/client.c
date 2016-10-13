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
#define SIZE 11

void func2(int sd){
	int i;
	int ret;
	FILE *fp;
	long long time;
	struct pollfd pollfd_var;

	fp = fdopen(sd, "r+");
	if(fp == NULL){
		perror("fdopen()");
		exit(-2);
	}

	//client connect
	//poll POLLOUT
	pollfd_var.fd = sd;
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

	//poll POLLOUT POLLIN
	while (1) {
		pollfd_var.fd = sd;
		pollfd_var.events = POLLIN|POLLOUT|POLLPRI;
		printf("block\n");
		ret = poll(&pollfd_var, 1, -1);
		if (ret == -1) {
			perror("poll()");
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
			//fscanf
			if(fscanf(fp, FMT_STAMP, &time) <0){
				perror("fscanf()");
				exit(-2);
			}else{
				printf("%lld\n", time);
				break;
			}
		}
		sleep(1);
		printf("sleep 1\n");
	}
	printf("-------------------------------\n");
	while (1) {
		pollfd_var.fd = sd;
		pollfd_var.events = POLLIN|POLLOUT|POLLPRI;
		printf("block\n");
		ret = poll(&pollfd_var, 1, -1);
		if (ret == -1) {
			perror("poll()");
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
		//POLLRDHUP 必须上边定义_GNU_SOURCE
		if(pollfd_var.revents & POLLRDHUP){
			printf("POLLRDHUP\n");
		}
		sleep(1);
		printf("sleep 1\n");
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
		exit(-2);
	}

	func2(sd);

	return 0;
}
