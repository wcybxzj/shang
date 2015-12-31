#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include <proto.h>
#include "client.h"

//#include "../include/proto.h"

/*
 *	-M		指定多播组
 *	-P		指定收接端口
 *	-p		指定播放器
 *	-H		显示帮助
 * */

struct client_conf_st client_conf={\
			.mgroup = DEFAULT_MGROUP,\
			.rcvport = DEFAULT_RCVPORT,\
			.player = DEFAULT_PLAYER};

static int deal_data(int fd);

int main(int argc,char **argv)
{
	int pipefd[2];
	pid_t pid;

	/*pipe*/
	if (pipe(pipefd) < 0) {
		perror("pipe");
		exit(1);
	}

	/*fork*/
	pid = fork();
	if (pid<0) {
		perror("fork()");
		exit(1);
	}

	if (pid==0) {
		close(pipefd[1]);
		dup2(pipefd[0], 0);
		if (pipefd[0] > 0) {
			close(pipefd[0]);
		}
		execl(client_conf.player, "mpg123","-",NULL);
		exit(0);
	}
	close(pipefd[0]);
	/*父进程接收网络上的包，交给子进程去播放*/
	deal_data(pipefd[1]);
	wait(NULL);
	exit(0);
}

static int deal_data(int fd){
	int sd;
	struct ip_mreqn req;
	struct sockaddr_in laddr, raddr;
	socklen_t raddr_len = sizeof(raddr);
	struct msg_list_st *list_buf;
	struct msg_channel_st *channel_buf;
	char *pos;
	int size;
	int current_channel = CHNID_LIST;

	//socket
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0) {
		perror("socket()");
		exit(1);
	}

	inet_pton(AF_INET, client_conf.mgroup, &req.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0", &req.imr_address);
	req.imr_ifindex = if_nametoindex("eth0");
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req)) < 0) {
		perror("setsockopt");
		exit(2);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(client_conf.rcvport));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	if (bind(sd, (void*)&laddr, sizeof(laddr))< 0) {
		perror("bind");
		exit(1);
	}

	list_buf = malloc(MAXENTRY);
	channel_buf = malloc(MAXDATA);
	while (1) {
		if (current_channel == CHNID_LIST) {
			memset(list_buf, 0x00, MAXENTRY);
			size = recvfrom(sd, list_buf, MAXENTRY, 0 ,(void*)&raddr, &raddr_len);
			if (size < 0) {
				perror("recvfrom");
				exit(1);
			}
			if (list_buf->chnid == CHNID_LIST) {
				pos = list_buf->entry;
				while (size > 1) {
					printf("%d\n", *pos);
					pos++; size--;
					printf("%s\n", pos);
					size -= strlen(pos)+1;
					pos += strlen(pos)+1;
				}
				printf("plz choose\n");
				if (scanf("%d", &current_channel) <= 0) {
					continue;
				}
			}
		}else{
			memset(channel_buf, 0x00, MAXDATA);
			size = recvfrom(sd, channel_buf, MAXDATA, 0 ,(void*)&raddr, &raddr_len);
			//printf("%d\n",size);
			if (size < 0) {
				perror("recvfrom");
				exit(1);
			}
			if (channel_buf->chnid == current_channel) {
				write(fd, channel_buf->data, size-1);
			}
		}
	}

	free(list_buf);
	free(channel_buf);

	return 0;
}
