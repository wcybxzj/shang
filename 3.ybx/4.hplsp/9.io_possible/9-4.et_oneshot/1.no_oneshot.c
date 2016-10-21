#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10
#define ONESHOT 1
#define NO_ONESHOT 0

struct fds_st {
   int epfd;
   int sockfd;
};

pid_t gettid()
{
     return syscall(SYS_gettid);
}

void setnonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
}

void addfd(int epollfd, int fd, int oneshot){
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	if (oneshot == ONESHOT) {
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnonblocking(fd);
}

void *worker1(void *ptr){
	printf(">>>start new thread:%d<<<\n", gettid());
	char buf[BUFFER_SIZE];
	int ret;
	int epfd = ((struct fds_st *) ptr)->epfd;
	int sockfd = ((struct fds_st *) ptr)->sockfd;
	while (1) {
		bzero(buf, BUFFER_SIZE);
		ret = recv(sockfd, buf, BUFFER_SIZE, 0);
		if (ret==0){
			printf("thread:%d EOF, remote client close socket\n", gettid());
			break;
		}else if(ret<0){
			if (errno == EAGAIN) {
				printf("thread:%d ret<0  and EAGAIN\n", gettid());
				break;
			}else{
				printf("thread:%d ret<0\n", gettid());
				break;
			}
		}else{
			printf("thread:%d got len:%d,content:%s\n", gettid(), ret, buf);
			sleep(1);//模拟这里有个业务处理
		}
	}
	printf("thread:%d,end of thread\n", gettid());
}

void func1(int listenfd)
{
	pthread_t tid;
	int i, ret, epfd, sockfd, connfd;
	struct epoll_event events[MAX_EVENT_NUMBER];
	struct sockaddr_in client_address;
	struct fds_st fds;
	socklen_t client_addrlength = sizeof(client_address);
	epfd = epoll_create(1);
	if (epfd == -1) {
		perror("epoll_create");
		exit(1);
	}
	addfd(epfd, listenfd, NO_ONESHOT);

	while (1) {
		ret = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1);
		if (ret < 0) {
			perror("epoll_wait");
			exit(1);
		}
		for (i = 0; i < ret; i++) {
			sockfd = events[i].data.fd;
			if (listenfd == sockfd) {
				printf("listenfd == sockfd\n");
				connfd = accept(sockfd, \
						(struct sockaddr *)&client_address, \
						&client_addrlength);
				if (connfd == -1) {
					perror("accept()");
					exit(1);
				}
				addfd(epfd, connfd, NO_ONESHOT);
			}else if(events[i].events & EPOLLIN){
				fds.epfd = epfd;
				fds.sockfd = sockfd;
				pthread_create(&tid, NULL, worker1, (void*) &fds);
			}else{
				printf("error\n");
				exit(1);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc!=3) {
		printf("usage:./%s ip port\n", basename(argv[0]));
		exit(1);
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	int reuse =1;
	int ret = 0;
	int listenfd;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if (ret == -1) {
		perror("setsockopt");
		exit(1);
	}
	ret = bind(listenfd, \
			(struct sockaddr* )&address, \
			sizeof(address));
	if (ret == -1) {
		perror("bind()");
		exit(1);
	}
	ret = listen(listenfd, 5);
	if (ret == -1) {
		perror("listen()");
		exit(1);
	}

	func1(listenfd);
	return 0;
}

//期望:一个fd在同一时间只有一个线程操作
//实验代码:
//server 每次recv > 0后sleep 2秒
//server端accept后的connfd不使用EPOLLONESHOT

//===========================================================================================

/*
//情况1:(没问题)
//客户端1次输入多长的数据, server创建1个线程,对connfd的事件都能在一个线程中多次recv来读取全,
//如果server EAGAIN后线程退出,如果客户端再发数据 server再次创建新线程来响应

终端1:
两个线程对1个fd的两次长输入分别相应
./1.no_oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:43139<<<
thread:43139 got len:10,content:1111111111  
thread:43139 got len:10,content:1222222222 (2秒)
thread:43139 got len:10,content:2333333333 (2秒)
thread:43139 got len:4,content:333

thread:43139 ret<0  and EAGAIN
thread:43139,end of thread
>>>start new thread:43148<<<
thread:43148 got len:10,content:4444444444
thread:43148 got len:10,content:5555555555 (2秒)
thread:43148 got len:10,content:5566666666 (2秒)
thread:43148 got len:3,content:66

thread:43148 ret<0  and EAGAIN
thread:43148,end of thread
>>>start new thread:43149<<<
thread:43149 EOF, remote client close socket
thread:43149,end of thread

终端2:
nc 127.0.0.1 1234
111111111112222222222333333333333
44444444445555555555556666666666
^C
*/

//===========================================================================================

/*
//情况2:(有问题)
//客户端多次频繁不间断write 20次,server 就会创建多个线程,出现多个线程对1个connfd共同读取
终端1:
./1.no_oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:46910<<<
thread:46910 got len:10,content:0,1,2,3,4,
>>>start new thread:46911<<<
thread:46911 got len:10,content:5,6,7,8,9,
thread:46910 got len:10,content:10,11,12,1
thread:46911 got len:10,content:3,14,15,16
thread:46910 got len:10,content:,17,18,19,
thread:46911 EOF, remote client close socket
thread:46911,end of thread
thread:46910 EOF, remote client close socket
thread:46910,end of thread

终端2:
连续不断write20次,每次2字节
./3.client 127.0.0.1 1234
*/

//===========================================================================================
/*
//情况3:(没问题)
终端1:
./1.no_oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:46954<<<
thread:46954 got len:2,content:0,
thread:46954 ret<0  and EAGAIN
thread:46954,end of thread
>>>start new thread:46955<<<
thread:46955 got len:2,content:1,
thread:46955 ret<0  and EAGAIN
thread:46955,end of thread
>>>start new thread:46956<<<
thread:46956 got len:2,content:2,
thread:46956 ret<0  and EAGAIN
thread:46956,end of thread
>>>start new thread:46957<<<
thread:46957 got len:2,content:3,
thread:46957 ret<0  and EAGAIN
thread:46957,end of thread
>>>start new thread:46958<<<
thread:46958 got len:2,content:4,
>>>start new thread:46959<<<
thread:46959 EOF, remote client close socket
thread:46959,end of thread
thread:46958 EOF, remote client close socket
thread:46958,end of thread

终端2:
使用4.client_sleep write20次每次中间间隔2秒
./4.client_sleep 127.0.0.1 1234
*/
