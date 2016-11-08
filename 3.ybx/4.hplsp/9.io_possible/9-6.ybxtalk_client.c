#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <poll.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

#define BUFFER_SZIE 6 //为epoll et+非阻塞io 缩小buff空间

int setnonblocking(int fd){
	int old = fcntl(fd, F_GETFL);
	int new = old | O_NONBLOCK;
	fcntl(fd, F_SETFL, new);
	return old;
}

//1.epoll et模式 + io nonblocking,使用et模式确实提高效率较少去epoll_wait的次数
//2.connect + io nonblocking,本例使用nonblocking connect不能提高效率纯粹练手
void epoll_version(int argc, const char *argv[])
{
	if (argc!=3) {
		printf("./a.out ip port\n");
		exit(1);
	}

	int epfd;
	struct epoll_event ev;
	char buf[BUFFER_SZIE];

	int sockfd, ret;
	const char *ip = argv[1];
	int port = atoi(argv[2]);
	struct sockaddr_in address;

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd<0) {
		perror("scoket");
		exit(1);
	}

	epfd = epoll_create(1);
	if (epfd<0) {
		perror("epoll_create()");
		exit(1);
	}

	//nonblocking connect
	int error, error_size;
	error_size = sizeof(error);
	setnonblocking(sockfd);
	ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
	if (ret < 0) {
		if (errno != EINPROGRESS) {
			perror("connect");
			exit(1);
		}else{
			//printf("EINPROGRESS\n");
		}
		ev.data.fd = sockfd;
		ev.events = EPOLLIN|EPOLLOUT;
		epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
		while (1) {
			ret = epoll_wait(epfd, &ev, 1, -1);
			if (ret == -1) {
				if (errno == EINTR) {
					printf("EINTR\n");
					continue;
				}else{
					perror("epoll_wait");
					exit(1);
				}
			}
			else{
				break;
			}
		}

		if (ev.events & EPOLLIN && ev.events & EPOLLOUT) {
			ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &error_size);
			if (ret < 0|| error != 0) {
				printf("nonblocking connect error:%s\n", strerror(error));
				exit(1);
			}
		}
		epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
	}

	setnonblocking(STDIN_FILENO);
	setnonblocking(sockfd);

	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN|EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

	ev.data.fd = sockfd;
	ev.events = EPOLLIN|EPOLLRDHUP|EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	while (1) {
		ret = epoll_wait(epfd, &ev, 1, -1);
		if (ret == -1) {
			perror("epoll_wait()");
			break;
		}

		//sockfd
		if (ev.data.fd == sockfd && ev.events & EPOLLRDHUP) {
			printf("server close the connection\n");
			break;
		}else if (ev.data.fd == sockfd && ev.events & EPOLLIN) {
			while (1) {
				bzero(buf, sizeof(buf));
				ret = recv(sockfd, buf, sizeof(buf)-1 ,0);
				if (ret<0) {
					if (errno == EAGAIN) {
						break;
					}else{
						perror("recv");
						exit(1);
					}
				}
				printf("%s",buf);
			}
		}

		//STDIN_FILENO
		if (ev.data.fd ==STDIN_FILENO && ev.events & EPOLLIN) {
			while (1) {
				bzero(buf, sizeof(buf));
				ret = read(STDIN_FILENO, buf, sizeof(buf)-1);
				if (ret<0) {
					if (errno == EAGAIN) {
						//printf("EAGAIN\n");
						break;
					}else{
						perror("read");
						exit(1);
					}
				}else if(ret ==0){
					printf("不应该运行到此 前面有EPOLLRDHUP\n");
				}

				ret = write(sockfd, buf, strlen(buf));
				if (ret<=0) {
					perror("write");
					break;
				}
			}
		}
	}
	close(sockfd);
}

//1.poll
//2.pipe和spilce,可能用splice进行零拷贝可以提高一定效率
void poll_version(int argc, const char *argv[])
{
	if (argc!=3) {
		printf("./a.out ip port\n");
		exit(1);
	}

	struct pollfd fds[2];
	char buf[BUFFER_SZIE];
	int pipefd[2];
	int sockfd, ret;
	const char *ip = argv[1];
	int port = atoi(argv[2]);
	struct sockaddr_in address;

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd<0) {
		perror("scoket");
		exit(1);
	}

	ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
	if (ret < 0) {
		perror("connect");
		exit(1);
	}

	ret = pipe(pipefd);
	if (ret == -1) {
		perror("pipe");
		exit(1);
	}

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = sockfd;
	fds[1].events = POLLIN|POLLRDHUP;
	fds[1].revents = 0;

	while (1) {
		bzero(buf, sizeof(buf));
		ret  = poll(fds, 2, 20*1000);//1second == 1000milliseconds
		if (ret == 0) {
			printf("client poll timeout\n");
			continue;
		}else if(ret < 0){
			perror("poll");
			exit(1);
		}

		//sockfd
		if (fds[1].revents& POLLRDHUP) {
			printf("server close the connection\n");
			break;
		}else if (fds[1].revents & POLLIN) {
			ret = recv(sockfd, buf, sizeof(buf)-1 ,0);
			printf("%s\n",buf);
		}

		//STDIN_FILENO
		if (fds[0].revents & POLLIN) {
			ret = splice(STDIN_FILENO, NULL, pipefd[1], NULL,\
					32768, SPLICE_F_MORE|SPLICE_F_MOVE);
			//printf("spilce ret:%d\n", ret);
			if (ret<0) {
				perror("splice");
				exit(1);
			}
			ret = splice(pipefd[0], NULL, sockfd, NULL,\
					32768, SPLICE_F_MORE|SPLICE_F_MOVE);
			//printf("spilce ret:%d\n", ret);
			if (ret<0) {
				perror("splice");
				exit(1);
			}
		}
	}
	close(sockfd);
}

int main(int argc, const char *argv[])
{
	epoll_version(argc, argv);
	//poll_version(argc, argv);
	return 0;
}
