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

void reset_oneshot(int epollfd, int fd){
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET|EPOLLONESHOT;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void *worker2(void *ptr){
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
				reset_oneshot(epfd, sockfd);
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

void func2(int listenfd)
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
				addfd(epfd, connfd, ONESHOT);//!!!!!!!!!!!
			}else if(events[i].events & EPOLLIN){
				fds.epfd = epfd;
				fds.sockfd = sockfd;
				pthread_create(&tid, NULL, worker2, (void*) &fds);
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

	func2(listenfd);

	return 0;
}


//===========================================================================================
//EPOLLONESHOT的意义:
//fd被设置成EPOLLONESHOT,意味着在epoll_wait只能响应一次 然后就处于非激活状态，就是即使再有事件
//fd也不去响应从而在此属性下fd操作不存在多线程并发操作
//如果想从这种非激活状态恢复到激活状态需要reset_oneshot fd
//===========================================================================================
//期望:一个fd在同一时间只有一个线程操作
//实验代码:
//server 每次recv后sleep 1秒
//server端accept后的connfd使用EPOLLONESHOT
//server在worker线程recv EAGAIN使用reset_oneshot
//=============================================================

/*
//情况1:(没问题)
终端1:
./2.oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:46060<<<
thread:46060 got len:10,content:1111111112
thread:46060 got len:10,content:2222222223
thread:46060 got len:10,content:3333333333
thread:46060 got len:8,content:4444444

thread:46060 ret<0  and EAGAIN
thread:46060,end of thread
>>>start new thread:46063<<<
thread:46063 got len:10,content:5555555555
thread:46063 got len:10,content:5566666666
thread:46063 got len:8,content:6677777

thread:46063 ret<0  and EAGAIN
thread:46063,end of thread
>>>start new thread:46101<<<
thread:46101 EOF, remote client close socket
thread:46101,end of thread

终端2:
nc 127.0.0.1 1234
1111111112222222222333333333334444444
555555555555666666666677777
^C
*/

//==================================================================
/*
//情况2:(没问题) 
终端1:
server worker 每次recv > 0 sleep1秒
/2.oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:46136<<<
thread:46136 got len:10,content:0,1,2,3,4,
thread:46136 got len:10,content:5,6,7,8,9,
thread:46136 got len:10,content:10,11,12,1
thread:46136 got len:10,content:3,14,15,16
thread:46136 got len:10,content:,17,18,19,
thread:46136 EOF, remote client close socket
thread:46136,end of thread

终端2:
连续不断write20次
./3.client 127.0.0.1 1234
*/
//==================================================================
/*
情况3:(没问题)
终端1:
server worker 每次recv > 0 sleep1秒
./2.oneshot 127.0.0.1 1234
listenfd == sockfd
>>>start new thread:46772<<<
thread:46772 got len:2,content:0,
thread:46772 ret<0  and EAGAIN
thread:46772,end of thread
>>>start new thread:46773<<<
thread:46773 got len:2,content:1,
thread:46773 ret<0  and EAGAIN
thread:46773,end of thread
>>>start new thread:46774<<<
thread:46774 got len:2,content:2,
thread:46774 ret<0  and EAGAIN
thread:46774,end of thread
>>>start new thread:46775<<<
thread:46775 got len:2,content:3,
thread:46775 EOF, remote client close socket
thread:46775,end of thread

终端2:
每2秒write 2个字节
./4.client_sleep 127.0.0.1 1234
^C
*/
