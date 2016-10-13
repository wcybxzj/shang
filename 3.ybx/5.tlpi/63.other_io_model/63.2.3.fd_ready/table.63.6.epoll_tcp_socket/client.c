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
#define SIZE 11

#define BUF_SIZE 1024
void func2(int sd){
	int i;
	int ret;
	char str[BUF_SIZE];
	long long time;
	struct pollfd pollfd_var;


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

	////poll POLLOUT POLLIN
	//pollfd_var.fd = sd;
	//pollfd_var.events = POLLIN|POLLOUT|POLLPRI;
	//while (1) {
	//	printf("block\n");
	//	ret = poll(&pollfd_var, 1, -1);
	//	if (ret == -1) {
	//		perror("poll()");
	//		exit(1);
	//	}
	//	if (pollfd_var.revents & POLLERR) {
	//		printf("POLLERR\n");
	//	}
	//	if(pollfd_var.revents & POLLOUT){
	//		printf("POLLOUT\n");
	//	}
	//	if(pollfd_var.revents & POLLIN){
	//		printf("POLLIN\n");
	//		ret = read(sd, str, BUF_SIZE);
	//		printf("ret:%d\n", ret);
	//		if (ret == -1) {
	//			perror("read");
	//			exit(1);
	//		}
	//		printf("%s\n",str);
	//	}
	//	sleep(1);
	//	printf("sleep 1\n");
	//}

	//epoll EPOLLIN EPOLLIN
	printf("-------------------------------\n");
	struct epoll_event ev;
	int epfd;
	epfd = epoll_create(1);
	if (epfd < 0) {
		perror("epoll_create():");
		exit(1);
	}

	ev.data.fd = sd;
	ev.events = EPOLLIN|EPOLLONESHOT;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sd, &ev);
	while (1) {
		printf("block\n");
		ret = epoll_wait(epfd,&ev ,1, -1);
		if (ret == -1) {
			perror("epoll()");
			exit(1);
		}

		if (ev.data.fd = sd && ev.events & EPOLLERR) {
			printf("EPOLLERR\n");
		}
		if (ev.data.fd = sd && ev.events & EPOLLIN) {
			printf("EPOLLIN\n");
			ret = read(ev.data.fd, str, BUF_SIZE);
			printf("ret:%d\n", ret);
			if (ret == -1) {
				perror("read");
				exit(1);
			}
			printf("%s\n",str);
		}
		if (ev.data.fd = sd && ev.events & EPOLLOUT) {
			printf("EPOLLOUT\n");
		}
		if (ev.data.fd = sd && ev.events & EPOLLRDHUP) {
			printf("EPOLLRDHUT\n");
		}
		if (ev.data.fd = sd && ev.events & EPOLLHUP) {
			printf("EPOLLHUP\n");
			break;
		}

		sleep(1);
		printf("sleep 1\n");
	}
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
