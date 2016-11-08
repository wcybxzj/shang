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

#define MAX_EVENT_NUMBER 1024
#define BUF_SIZE 1024
int pipefd[2];

int setnonblocking(int fd){
	int old = fcntl(fd, F_GETFL);
	int new = old|O_NONBLOCK;
	fcntl(fd, F_SETFL, new);
	return old;
}

void addfd(int epfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

void delfd(int epfd, int fd)
{
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, NULL );
}

void sig_handler(int sig){
	int save_errno = errno;
	int ret;
	char buf[BUF_SIZE]={0};
	ret = snprintf(buf, BUF_SIZE, "%d",sig);
	if (ret <=0) {
		perror("snprintf");
		exit(1);
	}
	send( pipefd[1], (char *)&buf, strlen(buf)+1, 0);
	errno = save_errno;
}

void addsig(int sig)
{
	int ret;
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;
	//相应此信号时候所有信号都被屏蔽
	sigfillset(&sa.sa_mask);
	ret = sigaction(sig, &sa, NULL);
	if (ret == -1) {
		perror("sigaction");
		exit(1);
	}
}

int main(int argc, char *argv[])
{
	if (argc <= 2) {
		printf("usage: %s ip port\n", basename(argv[0]));
		exit(1);
	}

	printf("pid:%d\n", getpid());

	char *ip = argv[1];
	int port = atoi(argv[2]);

	int i, ret;
	int tcp_listenfd, udp_fd, connfd;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	//tcp
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

	//udp
	udp_fd= socket(AF_INET, SOCK_DGRAM, 0);
	printf("udp_fd:%d\n", udp_fd);
	if (udp_fd == -1) {
		perror("socket");
		exit(1);
	}
	ret = bind(udp_fd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		perror("bind");
		exit(1);
	}

	//epoll
	int epfd;
	struct epoll_event events[MAX_EVENT_NUMBER];
	epfd  = epoll_create(5);
	if (epfd == -1) {
		perror("epoll_create");
		exit(1);
	}
	addfd(epfd, tcp_listenfd);
	addfd(epfd, udp_fd);

	//unix scoket
	ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
	if (ret == -1) {
		perror("socketpair");
		exit(1);
	}
	setnonblocking(pipefd[1]);
	addfd(epfd, pipefd[0]);

	//signal
	addsig(SIGINT);
	addsig(SIGHUP);

	int sockfd;
	struct sockaddr_in client_address;
	socklen_t client_len;
	char buf[BUF_SIZE];
	int len;
	int stop_serer = 0;
	while ( !stop_serer ) {
		printf("block\n");
		ret = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1);
		printf("unblock ret:%d\n", ret);
		if (ret == -1) {
			if (errno == EINTR) {
				continue;
			}
			perror("epoll_wait");
			exit(1);
		}

		for (i = 0; i < ret; i++) {
			sockfd = events[i].data.fd;
			printf("sockfd:%d\n", sockfd);

			if (sockfd == tcp_listenfd) {
				connfd = accept(tcp_listenfd, (struct sockaddr *)&client_address,\
						&client_len);
				addfd(epfd, connfd);
			}else if(sockfd == udp_fd){
				len = recvfrom(udp_fd, buf, sizeof(buf), 0,\
							(struct sockaddr*) &client_address, &client_len);
				if (len>0) {
					sendto(udp_fd, buf, len, 0, \
							(struct sockaddr*) &client_address, client_len);
				}else if (len<0) {
					if (errno == EAGAIN) {
						printf("EAGAIN\n");
						continue;
					}
					perror("recvfrom");
					exit(1);
				}else if(len==0) {
					printf("EOF\n");
					close(udp_fd);
					delfd(epfd, udp_fd);
					continue;
				}
			}else if (sockfd == pipefd[0]) {
				len = recv(sockfd, buf, sizeof(buf), 0);
				printf("signal:%s\n", buf);
			}else{
				if(events[i].events & EPOLLIN){
					while (1) {
						bzero(buf, BUF_SIZE);
						len = recv(sockfd, buf, sizeof(buf), 0);
						if (len < 0) {
							if (errno == EAGAIN) {
								printf("EAGAIN\n");
								continue;
							}
						}else if(len == 0){
							printf("EOF\n");
							close(sockfd);
							delfd(epfd, sockfd);
							break;
						}else{
							send(sockfd, buf, len, 0);
							break;
						}
					}
				}
			}
		}
	}

	return 0;
}
