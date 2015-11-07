#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

#include <proto.h>
#include <client.h>

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

int main(int argc, const char *argv[])
{
	int pd[2];
	int val =1;
	struct sockaddr_in laddr;
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
	};

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
				printf("-M  --mgroup 指定多播组 \n\
						-P  --port 指定端口 \n\
						-p --player 指定播放器\n\
						-e --eth指定网卡\n\
						-H --help\n");
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
	inet_pton(AF_INET, clinet_conf.mgroup, &mreq.imr_multiaddr);//组播地址
	inet_pton(AF_INET,"0.0.0.0", &mreq.imr_address);//本地地址
	mreq.imr_ifindex = if_nametoindex(client_conf.eth);
	if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))　< 0 ){
		perror("setsockopt()");
		exit(1);
	}
	
	if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val))　< 0 ){
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
		exit
	}

	pid = fork();
	if (pid < 0) {
		perror("fork()");
		exit(1);
	}

	//child: mpg123
	if (pid == 0) {
		exit(0);
	}
	//parent：从网络收包,发送给子进程

	//收节目单

	//选频道

	//收频道包并且发送给子进程

	return 0;
}
