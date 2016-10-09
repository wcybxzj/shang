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
#include <pthread.h>
#include <sys/syscall.h>

#include "proto.h"
#define IP_SIZE 16

pid_t gettid()
{
     return syscall(SYS_gettid);
}

typedef struct {
	int sd;
	int newsd;
	struct sockaddr_in raddr;
	socklen_t rlen;
} SOCKET_DATA;

void *func2(void *p){
	SOCKET_DATA *ptr = (SOCKET_DATA *)p;
	char str[IP_SIZE]={'\0'};
	char ip[IP_SIZE]={'\0'};
	int len;
	len = sprintf(str, FMT_STAMP, (long long)time(NULL));
	if(send(ptr->newsd, str, len, 0) < 0){
		perror("send()");
		pthread_exit(NULL);
	}

	if(inet_ntop(AF_INET, (void *)&ptr->raddr.sin_addr, \
				ip, IP_SIZE) == NULL){
		perror("inet_ntop()");
		pthread_exit(NULL);
	}

	printf("tid:%d radd:%s rport:%d\n", \
			gettid(), ip, htons(ptr->raddr.sin_port));

	close(ptr->newsd);
}

//for i in {1..100}; do { ./client 127.0.0.1; }& done
int main(){
	struct sockaddr_in laddr;
	pthread_t tid;
	int err;
	SOCKET_DATA *ptr = NULL;
	ptr = malloc(sizeof(SOCKET_DATA));
	if (ptr == NULL) {
		perror("malloc()");
		exit(-2);
	}

	ptr->sd = socket(AF_INET, SOCK_STREAM, 0);
	if(ptr->sd < 0){
		perror("socket");
		exit(0);
	}

	int val=1;
	if(setsockopt(ptr->sd, SOL_SOCKET, SO_REUSEADDR,\
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

	if(bind(ptr->sd, (void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(0);
	}

	listen(ptr->sd, 200);
	ptr->rlen = sizeof(ptr->raddr);
	while (1) {
		ptr->newsd = accept(ptr->sd, (void *)&ptr->raddr, &ptr->rlen);
		if(ptr->newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		err = pthread_create(&tid, NULL, func2, (void *)ptr);
		if(err){
			perror("pthread_create()");
			exit(-2);
		}

		//close(ptr->newsd);
	}
	exit(0);
}
