#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "proto.h"

int main()
{
	int sd;
	struct msg_st sbuf;
	struct sockaddr_in raddr;
	
    sd = socket(AF_INET,SOCK_DGRAM,0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	struct ip_mreqn req;

	inet_pton(AF_INET,MGROUP,&req.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&req.imr_address);
	req.imr_ifindex = if_nametoindex("eth0");
	
	if(setsockopt(sd,IPPROTO_IP,IP_MULTICAST_IF,&req,sizeof(req)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	memset(&sbuf,'\0',sizeof(sbuf));
	strcpy(sbuf.name,"Alan");
	sbuf.math = htonl(rand()%100);
	sbuf.chinese = htonl(rand()%100);

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET,MGROUP,&raddr.sin_addr);

    if(sendto(sd,&sbuf,sizeof(sbuf),0,(void *)&raddr,sizeof(raddr)) < 0)
	{
		perror("sendto()");
		exit(1);
	}

	puts("ok!");

    close(sd);

    exit(0);
}





