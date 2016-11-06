#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <errno.h>
//同时最多服务5个客户进行聊天
#define USER_LIMIT 3
#define BUFFER_SIZE 128
#define FD_LIMITS 65535

struct client_data{
	int fd;
	struct sockaddr_in address;
	char *write_buf;
	char buf[BUFFER_SIZE];
};

int setnonblocking(int fd)
{
	int old = fcntl(fd, F_GETFL);
	int new = old | O_NONBLOCK;
	fcntl(fd, F_SETFL, new);
	return old;
}

int get_fds_index(struct client_data *fds, int connfd, int user_counter){
	int i;
	for (i = 1; i <= user_counter; i++) {
		if (connfd == fds[i].fd) {
			return i;
		}
	}
	return -1;
}

int main(int argc, const char *argv[])
{
	if (argc !=3) {
		printf("./a.out ip port\n");
		exit(1);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);
	
	int i, j, k, err, ret, epoll_ret, fd, listenfd;
	socklen_t err_len;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		perror("socket");
		exit(1);
	}

	int val=1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){ 
		perror("setsockopt()");
		exit(0);
	}

	ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		perror("bind");
		exit(1);
	}

	ret = listen(listenfd, 5);
	if (ret == -1) {
		perror("listen");
		exit(1);
	}

	char *info;
	int user_counter = 0;

	struct client_data  fds[USER_LIMIT+1];
	for (i = 0; i <= USER_LIMIT; i++) {
		fds[i].fd = -1;
	}
	fds[0].fd = listenfd;

	int epfd = epoll_create(1);
	if (epfd == -1) {
		perror("epoll_create");
		exit(1);
	}
	struct epoll_event ev, tmp_ev;
	struct epoll_event evlist[USER_LIMIT];
	ev.data.fd = listenfd;
	ev.events = EPOLLIN|EPOLLERR|EPOLLRDHUP;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	int connfd;
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);

	while (1) {
		//printf("block\n");
		epoll_ret = epoll_wait(epfd, evlist, USER_LIMIT, -1);
		//printf("unblock ret:%d\n", epoll_ret);
		if (epoll_ret == -1) {
			if (errno == EINTR) {
				printf("EINTR\n");
				continue;
			}else{
				perror("epoll_wait");
				exit(1);
			}
		}

		for (i = 0; i < epoll_ret; i++) {
			tmp_ev = evlist[i];

			//printf("debug i:%d, fd:%d\n",i, tmp_ev.data.fd);
			//if (tmp_ev.events & EPOLLIN) {
			//	printf("IN\n");
			//}
			//if(tmp_ev.events & EPOLLERR){
			//	printf("ERR\n");
			//}
			//if(tmp_ev.events & EPOLLRDHUP){
			//	printf("RDHUP\n");
			//}
			//if (tmp_ev.events & EPOLLOUT) {
			//	printf("OUT\n");
			//}

			if (tmp_ev.data.fd == listenfd && tmp_ev.events & EPOLLIN) {
				connfd = accept(listenfd, (struct sockaddr *) &client_address, \
						&client_addrlength);
				if (connfd == -1) {
					perror("accept");
					exit(1);
				}
				if (user_counter == USER_LIMIT) {
					info ="服务器服务客户数已达上限";
					printf("%s\n", info);
					send(connfd, info, strlen(info), 0);
					sleep(1);//如果不sleep上边的输出在客户端看不到
					close(connfd);
					continue;
				}
				user_counter++;
				fds[user_counter].fd = connfd;
				fds[user_counter].address = client_address;
				setnonblocking(connfd);
				ev.data.fd = connfd;
				ev.events = EPOLLIN|EPOLLRDHUP|EPOLLERR;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
				printf("come a new user, now have %d users\n", user_counter);
				//printf("user fd:%d\n", connfd);
			}else if(tmp_ev.events & EPOLLERR){
				printf("EPOLLERR\n");
				connfd = tmp_ev.data.fd;
				if (getsockopt(connfd, SOL_SOCKET, SO_ERROR,\
							(void *) &err, &err_len) == -1)
				{    
					perror("getsockopt");
					exit(1);
				}    
				printf("erron:%s\n",strerror(err));
				exit(1);
			}else if(tmp_ev.events & EPOLLRDHUP){
				connfd = tmp_ev.data.fd;
				close(connfd);
				j = get_fds_index(fds, connfd, user_counter);
				if (j==-1) {
					printf("can not get fds i\n");
					exit(1);
				}
				fds[j] = fds[user_counter];
				user_counter--;
				epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL);
				sleep(1);
				printf("EPOLLRDHUP, a client user_counter:%d\n", user_counter);
				printf("left fd:%d\n",connfd);
			}else if(tmp_ev.events & EPOLLIN){
				connfd = tmp_ev.data.fd;
				k = get_fds_index(fds, connfd, user_counter);
				if (k==-1) {
					printf("can not get fds i\n");
					exit(1);
				}
				while (1) {
					bzero(fds[k].buf, BUFFER_SIZE);
					ret = recv(connfd, fds[k].buf, BUFFER_SIZE, 0);
					printf("fd:%d,recv:%s\n",connfd , fds[k].buf);
					if (ret<0) {
						if (errno == EAGAIN) {
							sleep(1);
							printf("EAGAIN fd:%d\n", connfd);
							break;
						}else{
							perror("recv");
							exit(1);
						}
					}else if(ret ==0){
						printf("EOF\n");
						printf("不应该出现在这 前面有EPOLLRDHUP\n");
						exit(1);
					}else{
						for (j = 1; j <= user_counter; j++) {
							fd = fds[j].fd;
							if (fd == connfd) {
								continue;
							}
							//printf("to fd:%d\n", fd);
							ev.data.fd = fd;
							ev.events = EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLOUT;
							epoll_ctl(epfd, EPOLL_CTL_MOD,fd , &ev);
							fds[j].write_buf = fds[k].buf;
						}
						break;
					}
				}
			}else if(tmp_ev.events & EPOLLOUT){
				connfd = tmp_ev.data.fd;
				k = get_fds_index(fds, connfd, user_counter);
				if (k==-1) {
					printf("can not get fds i\n");
					exit(1);
				}
				if (! fds[k].write_buf) {
					ev.data.fd = connfd;
					ev.events = EPOLLIN|EPOLLRDHUP|EPOLLERR;
					epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ev);
					continue;
				}
				ret = send(connfd, fds[k].write_buf,\
						strlen(fds[k].write_buf), 0);
				if (ret == -1) {
					perror("send()");
					exit(1);
				}
				fds[k].write_buf = NULL;
				ev.data.fd = connfd;
				ev.events = EPOLLIN|EPOLLRDHUP|EPOLLERR;
				epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ev);
			}
		}
	}


	free(fds);
	return 0;
}
