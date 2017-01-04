#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <strings.h>
#include <sys/epoll.h>
#include <pthread.h>

#define TIMEOUT 5000 //5000毫秒=5秒
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

static int epollfd = 0;

int setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option|O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void addfd(int epollfd, int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		printf("./%s ip port\n", basename(argv[0]));
		exit(1);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	int ret;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		perror("socket");
		exit(1);
	}
	ret = bind(listenfd, (struct sockaddr *) &address, sizeof(address));
	if (ret < 0) {
		perror("bind()");
		exit(1);
	}
	ret = listen(listenfd, 5);
	if (ret < 0) {
		perror("listen");
		exit(1);
	}

	epoll_event events[MAX_EVENT_NUMBER];
	int epollfd = epoll_create(5);
	if (epollfd < 0) {
		perror("epoll_create");
		exit(1);
	}
	addfd(epollfd, listenfd);

	int timeout = TIMEOUT;
	time_t start;
	time_t end;

	int i, number;
	int sockfd, connfd;
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);
	char buf[BUFFER_SIZE];

recv:
	while (1) {
		start = time(NULL);
		number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, timeout);
		if ( (number<0) && (errno!=EINTR)) {
			printf("epoll_wait return fail\n");
			break;
		}

		//epoll_wait超时
		if (number == 0) {
			printf("epoll_wait timeout\n");
			continue;
		}

		end = time(NULL);
		timeout -= (end - start)*1000;
		if (timeout <= 0) {
			timeout = TIMEOUT;
		}

		for (i = 0; i < number; i++) {
			sockfd = events[i].data.fd;
			if (sockfd == listenfd) {
				connfd = accept(listenfd, 
						(struct sockaddr *) &client_address,
						&client_addrlength);
				addfd(epollfd, connfd);
			}else if(events[i].events & EPOLLIN){
				bzero(buf, BUFFER_SIZE);
				ret = recv(sockfd,buf, BUFFER_SIZE-1, 0);
				if (ret<0) {
					if (errno != EAGAIN) {
						perror("recv");
						exit(1);
					}
				}else if (ret==0){
					printf("remote client close\n");
					goto recv;
				}else{
					send(sockfd,buf, BUFFER_SIZE-1, 0);
				}
			}else{
				//other
			}
		}

	}
	close(listenfd);
	return 0;
}
