#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>

#define BUF_SIZE 1024
int connfd=-1;

void sig_urg(int sig)
{
	int ret;
	int save_errno = errno;
	char buf[BUF_SIZE];
	if (connfd ==-1) {
		return;
	}
	bzero(buf, BUF_SIZE);
	ret = recv(connfd, buf, BUF_SIZE, MSG_OOB);
	if (ret == -1) {
		perror("recv");
		exit(1);
	}
	printf("get oob %d bytes content: %s\n", ret, buf);
	errno = save_errno;
}

void addsig(int sig, void (* sig_handler)(int))
{
	int ret;
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);
	ret = sigaction(sig, &sa, NULL);
}


//./5-6.oobsend 127.0.0.1 1234
//send oob data out

//./10-3.sigurg 127.0.0.1 1234
//tcp_listenfd:3
//get oob 1 bytes content: c
//123ab
//123
//EOF

int main(int argc, char *argv[])
{
	if (argc <= 2) {
		printf("usage: %s ip port\n", basename(argv[0]));
		exit(1);
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	int ret;
	int tcp_listenfd;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	tcp_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("tcp_listenfd:%d\n", tcp_listenfd);
	if (tcp_listenfd == -1) {
		perror("socket");
		exit(1);
	}

	int val=1;
	ret = setsockopt(tcp_listenfd, SOL_SOCKET, SO_REUSEADDR, \
			&val, sizeof(val));
	if (ret == -1) {
		perror("setsockopt");
		exit(1);
	}

	ret = bind(tcp_listenfd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		perror("bind");
		exit(1);
	}
	ret = listen(tcp_listenfd, 5);
	if (ret == -1) {
		perror("listen");
		exit(1);
	}

	socklen_t address_len= sizeof(address);
	connfd = accept(tcp_listenfd, (struct sockaddr *)&address, &address_len);
	if (connfd < 0) {
		perror("accept");
		exit(1);
	}

	addsig(SIGURG, sig_urg);
	fcntl(connfd, F_SETOWN, getpid());
	char buf[BUF_SIZE];
	while (1) {
		bzero(buf, BUF_SIZE);
		ret =recv(connfd, buf, BUF_SIZE, 0);
		if (ret == 0) {
			printf("EOF\n");
			break;
		}else if(ret <0){
			perror("recv");
			exit(1);
		}else{
			printf("%s\n",buf);
		}
	}
	close(connfd);
	close(tcp_listenfd);

	return 0;
}
