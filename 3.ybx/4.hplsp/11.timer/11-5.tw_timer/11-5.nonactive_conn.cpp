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
#include "11-5.tw_timer.h"

#define TIMESLOT 1
#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024

static int pipefd[2];
static  time_wheel wheel;
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

void sig_handler(int sig)
{
	int save_errno = errno;
	send(pipefd[1], (char *)&sig, 1, 0);
	errno = save_errno;
}

void addsig(int sig)
{
	struct sigaction sa;
	bzero(&sa, sizeof(sa));
	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);
	if (sigaction(sig, &sa, NULL)<0) {
		perror("sigaction");
		exit(1);
	}
}

void timer_handler()
{
	wheel.tick();
	alarm(TIMESLOT);
}

void cb_func(client_data* user_data)
{
	if (!user_data) {
		perror("user_data");
		exit(1);
	}
	epoll_ctl(epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
	close(user_data->sockfd);
	printf("close fd:%d\n", user_data->sockfd);
}

//linux socket本来可以用KEEPALIVE来处理非活动连接
//但在本里中应用层自己来处理非活动连接


//服务端:
//./11-5.nonactive_conn 127.0.0.1 1234
//客户端:
//nc 127.0.0.1 1234
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

	int val = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))<0) {
		perror("setsockpit");
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

	ret = socketpair(PF_UNIX, SOCK_STREAM, 0, pipefd);
	if (ret < 0) {
		perror("socketpair");
		exit(1);
	}
	setnonblocking(pipefd[1]);
	addfd(epollfd, pipefd[0]);

	addsig(SIGALRM);
	addsig(SIGINT);
	bool stop_server = false;

	client_data *users = new client_data[FD_LIMIT];
	bool timeout = false;
	alarm(TIMESLOT);

	time_t cur;
	int i, number, sig;
	char signals[1024];
	int sockfd, connfd;
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);
	tw_timer* timer;
	while (!stop_server) {
		number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		if ((number<0) && (errno!=EINTR)) {
			printf("epoll_wait return fail\n");
			break;
		}

		for (i = 0; i < number; i++) {
			sockfd = events[i].data.fd;
			if (sockfd == listenfd) {
				connfd = accept(listenfd, 
						(struct sockaddr *) &client_address,
						&client_addrlength);
				addfd(epollfd, connfd);
				timer = wheel.add_timer(15);//10秒超时
				timer->cb_func = cb_func;
				timer->user_data = &users[connfd];
				users[connfd].timer = timer;
				users[connfd].address = client_address;
				users[connfd].sockfd = connfd;
				printf("add_timer fd:%d\n", connfd);
				//printf("add_timer ts:%d\n",timer->time_slot);
			}else if((sockfd == pipefd[0]) && (events[i].events & EPOLLIN)){
				ret = recv(pipefd[0], signals, sizeof(signals), 0);
				if (ret == -1) {
					perror("recv");
					exit(1);
				}else if(ret == 0){
					continue;//EOF
				}else{
					for (i = 0; i < ret; i++) {
						switch(signals[i]){
							case SIGALRM:
								timeout = true;
								break;
							case SIGINT:
								stop_server = true;
								break;
						}
					}
				}
			}else if(events[i].events & EPOLLIN){
				bzero(users[sockfd].buf, BUFFER_SIZE);
				ret = recv(sockfd, users[sockfd].buf, BUFFER_SIZE-1, 0);
				printf("get %d bytes, data: %s from fd:%d\n",
						ret, users[sockfd].buf, sockfd);
				timer = users[sockfd].timer;
				if (ret<0) {
					if (errno != EAGAIN) {
						cb_func(&users[sockfd]);
						if (timer) {
							wheel.del_timer(timer);
						}
					}
				}else if (ret==0){
					cb_func(&users[sockfd]);
					if (timer) {
						wheel.del_timer(timer);
					}
				}else{
					send(sockfd, users[sockfd].buf, BUFFER_SIZE-1, 0);
					if (timer) {
						printf("adjust_timer fd:%d, rotation:%d\n",
								timer->user_data->sockfd, timer->rotation);
						wheel.adjust_timer(timer);
						printf("adjust_timer fd:%d, rotation:%d\n",
								timer->user_data->sockfd, timer->rotation);
					}else{
						perror("not have timer");
						exit(1);
					}
				}
			}else{
				//other
			}
		}

		if (timeout) {
			timer_handler();
			timeout = false;
		}
	}
	close(listenfd);
	close(pipefd[0]);
	close(pipefd[1]);
	delete [] users;
	return 0;
}
