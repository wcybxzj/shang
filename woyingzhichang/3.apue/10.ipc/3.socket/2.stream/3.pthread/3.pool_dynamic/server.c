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
#define MAXCLIENT  20
#define MINSEPARATE 5
#define MAXSEPARATE 10

enum{
    STATE_IDLE,
    STATE_BUSY
};

typedef struct pool_st{
    pthread_t tid;
    int state;
}POOL_ST;

static struct pool_st *pool_arr;
static int idle_num=0;
static int busy_num=0;
static int sd;
static pthread_t  main_tid;

void *worker_loop(void *p);
void add_one_worker();
void del_one_worker();
void manage_workers();

pid_t gettid()
{
	return syscall(SYS_gettid);
}

void add_one_worker(){
	int i, err;
	pthread_t tid;
	if(idle_num+busy_num ==  MAXCLIENT){
		return;
	}
	for(i=0; i<MAXCLIENT; i++){
		if(pool_arr[i].tid == -1){
			break;
		}
	}
	idle_num++;
	err = pthread_create(&tid, NULL, worker_loop, (void *) i);
	if(err){
		perror("pthread_create()");
		exit(-2);
	}
	//main thread
	pool_arr[i].tid = tid;
}

void * worker_loop(void *p){
	int len;
	int newsd;
	char str[IP_SIZE]={'\0'};
	char ip[IP_SIZE]={'\0'};
	struct sockaddr_in raddr;
	socklen_t rlen;
	int num  = (int) p;
	while (1) {
		pool_arr[num].state = STATE_IDLE;
		pthread_kill(main_tid, SIGUSR2);

		newsd = accept(sd, (void *)&raddr, &rlen);
		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			pthread_exit(NULL);
		}

		len = sprintf(str, FMT_STAMP, (long long)time(NULL));
		if(send(newsd, str, len, 0) < 0){
			perror("send()");
			pthread_exit(NULL);
		}

		pool_arr[num].state = STATE_BUSY;
		pthread_kill(main_tid, SIGUSR2);
		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			pthread_exit(NULL);
		}

		//PRINtf("tid:%d radd:%s rport:%d\n", \
		//		gettid(), ip, htons(raddr.sin_port));
		
		sleep(5);
		close(newsd);
	}
	pthread_exit(NULL);
}

void del_one_worker(){
    int i;
    for (i = 0; i < MAXCLIENT; i++) {
        if (pool_arr[i].tid != -1) {
            pthread_cancel(pool_arr[i].tid);
            pool_arr[i].tid = -1;
			break;
        }
    }
}

void manage_workers(){
    int i;
    idle_num=0;
    busy_num=0;
    for (i = 0; i < MAXCLIENT; i++) {
        if (pool_arr[i].tid!=-1) {
            if(pthread_kill(pool_arr[i].tid, 0)){
                pool_arr[i].tid = -1;
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

//for i in {1..100}; do { ./client 127.0.0.1; }& done
int main(){
	struct sockaddr_in laddr;
	pthread_t tid;
	int i, err, sig;
	static sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
		perror("pthread_sigmask");
		pthread_exit(NULL);
	}   

	main_tid = pthread_self();

	pool_arr = malloc(sizeof(POOL_ST)*MAXCLIENT);
	if (pool_arr == NULL) {
		perror("malloc()");
		exit(-2);
	}

	for(i=0; i < MAXCLIENT;i++){
		pool_arr[i].tid = -1;
	}

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
	for (i = 0; i <MINSEPARATE ; i++) {
		add_one_worker();
	}

	while (1) {
		err = sigwait(&set, &sig);
		if (err) {
			perror("sigwait()");
			exit(-2);
		}   

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
            if (pool_arr[i].tid==-1) {
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
	pthread_exit(NULL);
}
