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
#include <sys/mman.h>

#include "proto.h"

#define IP_SIZE 16
#define MAXCLIENT  20
#define MINSEPARATE 5
#define MAXSEPARATE 10

enum{
	STATE_IDLE,
	STATE_BUSY
};

struct pool_st{
	pid_t pid;
	int state;
};

static struct pool_st *pool_arr;
static int idle_num=0;
static int busy_num=0;
static int sd;

int worker_loop(int num);
void add_one_worker();
void del_one_worker();
void manage_workers();

void user2_handler(int s){
	return;
}

void add_one_worker(){
	int i;
	if(idle_num+busy_num ==  MAXCLIENT){
		return;
	}

	for(i=0; i<MAXCLIENT; i++){
		if(pool_arr[i].pid == -1){
			break;
		}
	}

	idle_num++;
	pid_t pid;
	pid = fork();
	switch (pid){
		case 0:
			worker_loop(i);
			break;
		case -1:
			perror("fork()");
			exit(-1);
			break;
		default:
			pool_arr[i].pid = pid;
			break;
	}
}

void del_one_worker(){
	int i;
	for (i = 0; i < MAXCLIENT; i++) {
		if (pool_arr[i].pid != -1) {
			kill(pool_arr[i].pid, SIGTERM);
			pool_arr[i].pid = -1;
				break;
		}
	}
}

void manage_workers(){
	int i;
	idle_num=0;
	busy_num=0;
	for (i = 0; i < MAXCLIENT; i++) {
		if (pool_arr[i].pid!=-1) {
			if(kill(pool_arr[i].pid, 0)){
				pool_arr[i].pid = -1;
			}
			if (pool_arr[i].state == STATE_BUSY) {
				busy_num++;
			}
			if (pool_arr[i].state == STATE_IDLE) {
				idle_num++;
			}
		}
	}
}

int worker_loop(int num){
	char str[IP_SIZE]={'\0'};
	int len;
	int newsd;
	struct sockaddr_in raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	rlen = sizeof(raddr);

	while (1) {
		pool_arr[num].state = STATE_IDLE;
		kill(getppid(), SIGUSR2);

		newsd = accept(sd, (void *) &raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		pool_arr[num].state = STATE_BUSY;
		kill(getppid(), SIGUSR2);
		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}

		//printf("pid:[%d]radd:%s rport:%d\n", \
				getpid(), ip, htons(raddr.sin_port));

		len = sprintf(str, FMT_STAMP, (long long)time(NULL));
		if(send(newsd, str, len, 0) < 0){
			perror("send()");
			exit(-3);
		}
		close(newsd);
		sleep(5);
	}
	close(sd);
	exit(0);
}

int main(){
	int i;
	struct sockaddr_in laddr;
	struct sigaction sa;
	sigset_t set, oset;

	sa.sa_handler=user2_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=0;
	sigaction(SIGUSR2, &sa, NULL);

	sa.sa_handler=SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_NOCLDWAIT;
	sigaction(SIGCHLD, &sa, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &set, &oset);

	pool_arr = mmap(NULL, sizeof(struct pool_st)*MAXCLIENT, \
			PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

	if (pool_arr == MAP_FAILED) {
		perror("mmap()");
		exit(-2);
	}

	for(i=0; i<MAXCLIENT;i++){
		pool_arr[i].pid = -1;
	}

	sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/);
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
	printf("w\n");
	for(i=0; i<MINSEPARATE; i++){
		add_one_worker();
	}

	while (1) {
		sigsuspend(&oset);
		manage_workers();
		if(idle_num < MINSEPARATE){
			for(i=0; i<MINSEPARATE-idle_num; i++){
				add_one_worker();
			}
		}

		if(idle_num > MAXSEPARATE){
			for(i=0; i<idle_num-MAXSEPARATE; i++){
				del_one_worker();
			}
		}

		for (i = 0; i < MAXCLIENT; i++) {
			if (pool_arr[i].pid==-1) {
				write(1, " ", 1);
			}else{
				if (pool_arr[i].state == STATE_IDLE) {
					write(1, ".", 1);
				}else if(pool_arr[i].state == STATE_BUSY){
					write(1, "*", 1);
				}
			}
		}
		printf("\n");
	}

	exit(0);
}
