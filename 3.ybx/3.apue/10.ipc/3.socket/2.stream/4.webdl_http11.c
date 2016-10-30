#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define SIZE 1024
#define NUM 1024

void func_stdio(int sd){
	char buf[NUM] = {'\0'};
	FILE *fp = NULL;
	int len;
	fp = fdopen(sd, "r+");
	if (fp == NULL) {
		perror("fdopen()");
		exit(-3);
	}

	//fwrite
	fprintf(fp, "%s",
			"GET /1.jpg HTTP/1.1\r\nHost: 192.168.91.11:1234\r\n\r\n");

	while (1) {
		len = fread(buf, 1, NUM, fp);
		if (len<=0) {
			break;
		}
		fwrite(buf, 1, len, stdout);
	}
	fclose(fp);
}
//使用方式 ./webdl 127.0.0.1 > 1.jpg
int main(int argc, const char *argv[])
{
	int sd, len;
	struct sockaddr_in saddr;
	if(argc!=3){
		perror("./a.out ip port");
		exit(-1);
	}

	int port = atoi(argv[2]);

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		perror("socket");
		exit(-2);
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	if(inet_pton(AF_INET, argv[1], &saddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(connect(sd, (void *)&saddr, sizeof(saddr)) < 0){
		perror("connect()");
		exit(-2);
	}

	func_stdio(sd);
	return 0;
}
