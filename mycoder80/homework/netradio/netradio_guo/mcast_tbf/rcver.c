#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <arpa/inet.h>

#include "proto.h"

#define IPSTRSIZE	40

int main()
{
	int sd;
	struct sockaddr_in laddr,raddr;	
	socklen_t raddr_len;
	struct msg_chnnal_st *rbuf;
	char ipstr[IPSTRSIZE];
	int size;

	sd = socket(AF_INET,SOCK_DGRAM,0/*IPPROTO_UDP*/);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	struct ip_mreqn req;

    inet_pton(AF_INET,DEFAULT_MGROUP,&req.imr_multiaddr);
    inet_pton(AF_INET,"0.0.0.0",&req.imr_address);
    req.imr_ifindex = if_nametoindex("eth0");

    if(setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&req,sizeof(req)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(DEFAULT_RCVPORT));
	inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr.s_addr);
	if(bind(sd,(void *)&laddr,sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	raddr_len = sizeof(raddr);

	rbuf = malloc(MAXDATA);

	while(1)
	{
		size = recvfrom(sd,rbuf,MAXDATA,0,(void *)&raddr,&raddr_len);
		if(size < 0)
		{
			perror("recvfrom()");
			exit(1);
		}

		write(1, rbuf->data, size);
/*
		inet_ntop(AF_INET,&raddr.sin_addr.s_addr,ipstr,IPSTRSIZE);

		printf("----MESSAGE FROME:%s:%d----\n",ipstr,ntohs(raddr.sin_port));
		printf("NAME = %s\n",rbuf.name);
		printf("MATH = %d\n",ntohl(rbuf.math));
		printf("CHINESE = %d\n",ntohl(rbuf.chinese));
*/
	}
	
	close(sd);	

	exit(0);
}

