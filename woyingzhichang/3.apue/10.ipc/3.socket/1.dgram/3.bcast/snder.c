#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "proto.h"
int main(int argc, char * argv[]){
	int sd, len, sbuf_len;
	struct msg_st* sbuf;
	struct sockaddr_in saddr;

	if(argc != 2){
		perror("usage:./snder ybx");
		exit(-1);
	}

	srand(time(NULL));
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd <0){
		perror("socket()");
		exit(-1);
	}
	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST, \
				&val, sizeof(val)) < 0){
		perror("setsockopt()");
		exit(-1);
	}

	sbuf_len = strlen(argv[1])+sizeof(struct msg_st);
	sbuf = malloc(sbuf_len);
	if(sbuf == NULL){
		perror("malloc()");
		exit(-1);
	}
	memset(sbuf->name, '\0', NAMESIZE);
	strncpy(sbuf->name, argv[1], NAMESIZE);
	sbuf->math = htonl(rand()%100);
	sbuf->chinese = htonl(rand()%100);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET,"255.255.255.255", &saddr.sin_addr) !=1){
		perror("inet_pton()");
		exit(1);
	}

	len = sendto(sd, sbuf, sbuf_len, 0, (void *)&saddr, sizeof(saddr) );
	if(len < 0){
		perror("sendto()");
		exit(0);
	}
	printf("ok\n");
	close(sd);
	exit(0);
}
