#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <proto.h>
#include "client.h"

/*
 * -M  --mgroup 指定多播组
 * -P  --port 指定端口
 * -p --player 指定播放器
 * -e --eth  设置网卡
 * -H --help
 */

struct client_conf_st client_conf = {
	.rcvport = DEFAULT_RCVPORT,
	.mgroup = DEFAULT_MGROUP,
	.player_cmd = DEFAULT_PLAYERCMD,
	.eth = DEFAULT_EHT
};

//坚持向fd写len个字节
static ssize_t writen(int fd, const char *buf, size_t len){
	int pos = 0;
	int ret;
	while (len > 0) {
		ret = write(fd, buf + pos, len);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("write()");
			return -1;
		}
		len -= ret;
		pos += ret;
	}
	return pos;
}

static void printhelp(void)
{
	printf("-M  --mgroup 指定多播组 \n\
			-P  --port 指定端口 \n\
			-p --player 指定播放器\n\
			-e --eth指定网卡\n\
			-H --help\n");
}

int main(int argc, char *argv[])
{
	int pid;
	int ret;
	int len;
	int chosenid;//选择的频道id
	int pd[2];
	int val =1;
	struct sockaddr_in laddr, serveraddr, raddr;
	socklen_t serveraddr_len, raddr_len;
	int c;
	int sd;
	int index = 0;
	struct ip_mreqn mreq;
	struct option argarr[] = {
		{"port", 1, NULL, 'P'},
		{"mgroup", 1, NULL, 'M'},
		{"player", 1, NULL, 'p'},
		{"eth", 1, NULL, 'e'},
		{"help", 1, NULL, 'H'},
		{NULL, 0, NULL, 0}
	};//最后一项必须为空

	/*初始化:
	 * 级别:默认值<配置文件<环境变量<命令行参数
	 */
	while (1) {
		c = getopt_long(argc, argv, "P:p:M:H", argarr, &index);
		if (c < 0) {
			break;
		}

		switch(c){
			case 'P':
				client_conf.rcvport = optarg;
				break;
			case 'M':
				client_conf.mgroup = optarg;
				break;
			case 'p':
				client_conf.player_cmd = optarg;
				break;
			case 'e':
				client_conf.eth = optarg;
				break;
			case 'H':
				printhelp();
				exit(0);
				break;
			default:
				break;
		}
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if (sd < 0) {
		perror("socket");
		exit(1);
	}

	//man 7 ip
	inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0", &mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(client_conf.eth);
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
		perror("setsockopt()");
		exit(1);
	}

	//无所谓的参数
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) < 0){
		perror("setsockopt()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(client_conf.rcvport));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	if(bind(sd,(void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(1);
	}

	if (pipe(pd) < 0){
		perror("pipe()");
		exit(1);
	}

	pid = fork();
	if (pid < 0) {
		perror("fork()");
		exit(1);
	}

	//child: mpg123
	if (pid == 0) {
		close(sd);//关闭不需要的文件描述符
		close(pd[1]);//0读,１写
		dup2(pd[0],0);//让标准输入成为管道的读端
		if (pd[0] > 0) {
			close(pd[0]);
		}
		if (execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL) < 0) {
			perror("execl()");
		}
		exit(1);
	}

	//parent：从网络收包,发送给子进程
	//收节目单
	struct msg_list_st *msg_list;
	msg_list = malloc(MSG_LIST_MAX);
	if (NULL == msg_list) {
		perror("malloc");
		exit(1);
	}

	while (1) {
		len = recvfrom(sd, msg_list, MSG_LIST_MAX,0, (void *)&serveraddr, &serveraddr_len);
		if (len < sizeof(struct msg_list_st)) {
			fprintf(stderr, "message is too small\n");
			continue;
		}
		if (msg_list->chnid != LISTCHNID) {//不是节目单
			fprintf(stderr, "chnid is not match\n");
			continue;
		}
		break;
	}

	//打印节目单并选择频道
	struct msg_listentry_st *pos;
	for (pos = msg_list->entry;
			(char *)pos < (((char *)msg_list)+len);
			pos = (void *) (((char *)pos) + ntohs(pos->len))) 
	{
		printf("chanel %d:%s\n",pos->chnid, pos->desc);
	}

	free(msg_list);
	while (1) {
		ret = scanf("%d", &chosenid);
		if (ret != 1) {
			exit(1);
		}
	};

	//收频道包 并且发送给子进程
	struct msg_channel_st *msg_channel;
	msg_channel = malloc(MSG_CHANNEL_MAX);
	if (NULL == msg_channel) {
		perror("malloc()");
		exit(1);
	}

	while (1) {
		len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &raddr_len);
		if (raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || \
				raddr.sin_port != serveraddr.sin_port) {
			fprintf(stderr, "接受节目包和频道包的ip或者port 不匹配\n");
			continue;
		}

		if (len < sizeof(struct msg_channel_st)) {
			fprintf(stderr, "channel message is too small\n");
			continue;
		}

		if (msg_channel->chnid == chosenid) {
			fprintf(stdout, "accepted msg:%d recieved.\n", msg_channel->chnid);
			if (writen(pd[1],msg_channel->data, len - sizeof(chnid_t)) < 0) {
				exit(1);
			}
			
		}
	}

	free(msg_channel);
	close(sd);

	return 0;
}
