#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <proto.h>

#include "thr_channle.h"
#include "medialib.h"

#define BUFSIZE		1024

int sd;
struct sockaddr_in raddr;
pthread_t tid[NR_CHN+1];

/*
 *  -M      指定多播组
 *  -P      指定收接端口
 *  -C		指定测试频道
 *  -F		前台调试运行
 *  -H      显示帮助
 * */

int main()
{
	struct mlib_chn_st *listptr;
	int listsize;
	int i, err;
	char buf[BUFSIZE];
	int len;
	struct ip_mreqn req;
	chnid_t id;

	/*conf处理*/

	/*socket init*/
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0) {
		perror("socket()");
		exit(1);
	}

	inet_pton(AF_INET, DEFAULT_MGROUP, &req.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &req.imr_address);
	req.imr_ifindex = if_nametoindex("eth1");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &req, sizeof(req)) < 0) {
		perror("setsockopt()");
		exit(1);
	}

/*	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(EFAULT_RCVPORT));
	inet_pton(AF_INET, DEFAULT_MGROUP, &raddr.sin_addr);
*/
	/*获取频道列表信息（从medialib中）*/
	err = mlib_getchnlist(&listptr,&listsize);		
	for(i = 0; i < listsize; i++) {
		printf("%d: ", listptr[i].chnid);
		puts(listptr[i].desc);
	}
/*	if(err)
	{
		syslog();
	}*/

	/*创建thr_list
	err = thr_list_create();
	if()
	{

	}
*/
	/*创建thr_channel*/
	/*1:200  100:200  4:200  200:200 */

	for(i = 1; i <= listsize; i++) {
		id = i;
		thr_channel_create(&id); // &((chnid_t)i) 单目&的操作数必须是左值报错 
	}
	while(1)
		pause();

	exit(0);
}


