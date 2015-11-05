#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "proto.h"

#define P_NUM 4

int main(int argc,char **argv)
{
	int i;
	int sd;
	FILE *fp;
	struct sockaddr_in raddr;
	long long stamp;
	pid_t pid;

	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	for (i = 0; i < P_NUM; i++) {
		printf("i : %d\n", i);
		pid = fork();
		if (pid==0) {
			sd = socket(AF_INET,SOCK_STREAM,0);
			if(sd < 0)
			{
				perror("socket()");
				exit(1);
			}

			raddr.sin_family = AF_INET; 
			raddr.sin_port = htons(atoi(SERVERPORT));
			inet_pton(AF_INET,argv[1],&raddr.sin_addr);
			if(connect(sd,(void *)&raddr,sizeof(raddr)) < 0)
			{
				perror("connect()");
				exit(1);
			}

			fp = fdopen(sd,"r+");
			if(fp == NULL)
			{
				perror("fdopen()");
				exit(1);
			}

			if(fscanf(fp,FMT_STAMP,&stamp) < 1)
				fprintf(stderr,"fscanf() failed.\n");
			else
				printf("stamp = %lld\n",stamp);

			fclose(fp);
			exit(0);
		}
	}

	for (i = 0; i < P_NUM; i++) {
		wait(NULL);
	}


	exit(0);
}


