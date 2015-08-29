#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#define BUFSIZE	1024

int main(int argc,char **argv)
{
	int sd;
	int err;
	struct sockaddr_in raddr;
	long long stamp;
	FILE *fp;
	int len;
	char rbuf[BUFSIZE];
	struct addrinfo aif,*res;

	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	aif.ai_flags = 0;
	aif.ai_family = AF_INET;
	aif.ai_socktype = SOCK_STREAM;
	aif.ai_protocol = 0;

	err = getaddrinfo(argv[1],"http",&aif,&res);
	if(err)
	{
		fprintf(stderr,"getaddrinfo():%s\n",gai_strerror(err));
		exit(1);
	}


	sd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(sd < 0)
    {
        perror("socket()");
        exit(1);
    }

	if(connect(sd,res->ai_addr,res->ai_addrlen) < 0)
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


	fprintf(fp,"GET /test.jpg\r\n\r\n");
	fflush(fp);

	while(1)
	{
		len = fread(rbuf,1,BUFSIZE,fp);
		if(len <= 0)
			break;
		fwrite(rbuf,1,len,stdout);
	}
	
	fclose(fp);

	freeaddrinfo(res);

	exit(0);
}


