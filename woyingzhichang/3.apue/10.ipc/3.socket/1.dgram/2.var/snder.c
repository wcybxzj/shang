#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "proto.h"
//snder.c 是主动端，主动向另一端发送消息。
//可以不用向操作系统绑定端口，
//发送数据的时候由操作系统为我们分配可用的端口即可，
//当然如果想要自己绑定特定的端口也是可以的。
int main(int argc, char * argv[]){
	int sd, len, sbuf_len;
	struct msg_st* sbuf;
	struct sockaddr_in saddr;

	if(argc != 3){
		perror("usage:./snder 127.0.0.1 ybx");
		exit(-1);
	}

	srand(time(NULL));
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd <0){
		perror("socket()");
		exit(-1);
	}

	sbuf_len = strlen(argv[2])+sizeof(struct msg_st);
	sbuf = malloc(sbuf_len);
	if(sbuf == NULL){
		perror("malloc()");
		exit(-1);
	}
	strncpy(sbuf->name, argv[2], NAMESIZE);
	sbuf->math = htonl(rand()%100);
	sbuf->chinese = htonl(rand()%100);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, argv[1], &saddr.sin_addr) !=1){
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
