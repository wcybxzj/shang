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
#define PTHREAD_NUM 4

pid_t gettid()
{
     return syscall(SYS_gettid);
}

int sd;

void * worker_loop(void *p){
	char str[IP_SIZE]={'\0'};
	char ip[IP_SIZE]={'\0'};
	int len;
	int newsd;
	struct sockaddr_in raddr;
	socklen_t rlen;

	while (1) {
		newsd = accept(sd, (void *)&raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		len = sprintf(str, FMT_STAMP, (long long)time(NULL));
		if(send(newsd, str, len, 0) < 0){
			perror("send()");
			pthread_exit(NULL);
		}

		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			pthread_exit(NULL);
		}

		printf("tid:%d radd:%s rport:%d\n", \
				gettid(), ip, htons(raddr.sin_port));

		close(newsd);
	}
	pthread_exit(NULL);
}

//for i in {1..100}; do { ./client 127.0.0.1; }& done
int main(){
	struct sockaddr_in laddr;
	pthread_t tid;
	int i, err;

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

	listen(sd, 200);
	for (i = 0; i < PTHREAD_NUM; i++) {
		err = pthread_create(&tid, NULL, worker_loop, NULL);
		if(err){
			perror("pthread_create()");
			exit(-2);
		}
	}
	pthread_exit(NULL);
}
