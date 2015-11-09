#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <proto.h>

#include "client.h"


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

static int getchndata(int fd);

int main(int argc,char **argv)
{
	int pd[2];
	pid_t pid;
	int ch;

	/*conf处理 getopt*/
	while(1) {
		ch = getopt(argc, argv, "M:P:p:");
		if(ch < 0) {
			break;
		}
		switch(ch) {
			case 'M':
				client_conf.mgroup = optarg;
				break;
			case 'P':
				client_conf.rcvport = optarg;
				break;
			case 'p':
				client_conf.player = optarg;
				break;
			default:
				break;
		}
	}
	/*pipe*/
	if(pipe(pd) < 0) {
		perror("pipe()");
		exit(1);
	}

	/*fork*/
	pid = fork();
	if(pid < 0) {
		perror("fork()");
		exit(1);
	}else if(!pid) {
		close(pd[1]);
		dup2(pd[0], 0);
		if(pd[0] > 0)
			close(pd[0]);
		execl(client_conf.player, "mpg123", "-", NULL);
		perror("execl()");
		exit(1);
	}
	close(pd[0]);
	
	/*父进程接收网络上的包，交给子进程去播放*/
	getchndata(pd[1]);

	wait(NULL);

	exit(0);
}


static int getchndata(int fd)
{
	int sd;
	struct sockaddr_in laddr,raddr;
	socklen_t raddr_len = sizeof(raddr);
	struct ip_mreqn req;
	struct msg_chnnal_st *rbuf_data;
	struct msg_list_st *rbuf_list;
	int size;
	char *pos;
	int channel = CHNID_LIST;
	
	/*socket init*/
	sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if(sd < 0) {
		perror("socket()");
		exit(1);
	}

	inet_pton(AF_INET,client_conf.mgroup,&req.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&req.imr_address);
	req.imr_ifindex = if_nametoindex("eth0");
	if(setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&req,sizeof(req)) < 0){
		perror("setsockopt()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(client_conf.rcvport));
	inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr.s_addr);
	if(bind(sd,(void *)&laddr,sizeof(laddr)) < 0) {
		perror("bind()");
		exit(1);
	}

	rbuf_data = malloc(MAXDATA);
	rbuf_list = malloc(MAXENTRY);
	while(1) {
		if(channel == CHNID_LIST) {
			memset(rbuf_list, 0x00, MAXENTRY);
			size = recvfrom(sd, rbuf_list, MAXENTRY, 0, (void *)&raddr, &raddr_len);
			if(size < 0) {
				perror("recvfrom()");
				exit(1);
			}
			if(rbuf_list->chnid == CHNID_LIST) {
				pos = rbuf_list->entry;
				for(;size > 1;) {
					printf("%d:", *pos);
					++pos; --size;
					printf("%s", pos);
					size -= strlen(pos)+1;
					pos += strlen(pos)+1;
				}
				printf("please choose the channel:\n");
				if(scanf("%d", &channel) <= 0)
					continue;
			}
		}else if((channel >= MINCHNID) && (channel <= MAXCHNID)) {
			memset(rbuf_data, 0x00, MAXDATA);
			size = recvfrom(sd, rbuf_data, MAXDATA, 0, (void *)&raddr,&raddr_len);
			if(size < 0) {
				perror("recvfrom()");
				exit(1);
			}
			if(channel == rbuf_data->chnid)
				write(fd, rbuf_data->data, size-1);
		}
	}
	free(rbuf_data);
	free(rbuf_list);

	return 0;
}



