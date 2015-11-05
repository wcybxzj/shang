#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>

#include "proto.h"

#define BUFSIZE 1024
#define STRSIZE 40

#define MAXCLIENT 20
#define MINSPARESERVER 5
#define MAXSPARESERVER 10

#define SIG_NOTIFY SIGUSR2

enum {
	STATE_IDLE = 0,
	STATE_BUSY
}

struct server_st{
	int pid;
	int state;
}

int sd;
static struct server_st * serverpool;
static int idle_count = 0;
static int busy_count = 0;

static void usr2_handler(int s){
	return;
}

static int add_1_server(void){
	int i;
	pid_t pid;
	if (idle_count+busy_count >= MAXCLIENT) {
		return -1;
	}
	for (i = 0; i < MAXCLIENT; i++) {
		if (serverpool[i].pid == -1) {
			break;
		}
	}
	pid = fork();
	if (pid == 0) {
		server_job(i);
	}
}

int main(int argc, const char *argv[])
{
	int i;
	char ipstr[STRSIZE];
	struct sockaddr_in laddr;
	struct sigaction sa,osa1,osa2;
	sigset_t set, oset;

	//signal
	sa.sa_handler = notify_handler;
	sigempty(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIG_NOTIFY, &sa, &osa1);

	sa.sa_handler = SIG_IGN;
	sigempty(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIG_NOTIFY, &sa, &osa2);

	sigemptyset(&set);
	sigaddset(&set, SIG_NOTIFY);
	sigprocmask(SIG_BLOCK, &set, &oset);

	//init
	serverpool = mmap(NULL, sizeof(struct server_st)*MAXCLIENT, PROT_READ|PROT_WRITE|, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (serverpool == MAP_FAILED) {
		perror("mmap()");
		exit(1);
	}

	for (i = 0; i < MAXCLIENT; i++) {
		serverpool[i].pid = -1;
	}

	//socket
	sd = ocket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("socket");
		exit(1);
	}

	int val =1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
		perror("setsockopt()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0) {
		perror("bind()");
		exit(1);
	}

	if (listen(sd, 200) < 0) {
		perror("listen()");
		exit(1);
	}

	for (i = 0; i < MINSPARESERVER; i++) {
		add_1_server();
	}

	while (1) {
		sigsuspend(&oset);

		scan_pool();

		if (idle_count > MAXSPARESERVER) {
			for (i = 0; i < idle_count - MAXSPARESERVER ; i++) {
				add_1_server();
			}
		}else if(idle_count < MINSPARESERVER){
			for (i = 0; i < MINSPARESERVER - idle_count; i++) {
				del_1_server();
			}
		}

		for (i = 0; i < MAXCLIENT; i++) {
			if (serverpool[i].pid == -1) {
				putchar('n');
			}else {
				if (serverpool[i].state == STATE_IDLE) {
					putchar('.');
				}else if(serverpool[i].state == STATE_BUSY){
					putchar('x');
				}
			}
		}
		printf("\n");
	}

	return 0;
}
