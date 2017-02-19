#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>

//epoll,select,poll:
//都支持管道，FIFO，套接字，POSIX消息队列，终端，设备等

//epoll:
//不支持普通文件或目录的fd，

//select,poll:
//支持普通文件或目录的fd，

//epoll不支持普通文件描述符
void epoll_normal_fd()
{
	int fd, ret;
	fd = open("/tmp/123",O_RDWR|O_CREAT,0666);
	if (fd==-1) {
		perror("open");
		exit(1);
	}

	int epfd;
	struct epoll_event ev;
	epfd = epoll_create(1);//传参正整数即可,无意义
	if (epfd < 0) {
		perror("epoll_create():");
		exit(1);
	}

	ev.events |= EPOLLIN;
	ev.data.fd = fd;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if (ret==-1) {
		perror("epoll_ctl");//epoll_ctl: Operation not permitted
		exit(1);
	}

	ret = epoll_wait(epfd, &ev, 1, -1);
	if (ret==-1) {
		perror("epoll_wait");
		exit(1);
	}
}//end epoll_normal_fd

//epoll支持pipe
void epoll_pipe()
{
	int pd[2], ret;
	if(pipe(pd) < 0){
		perror("perror():");
		exit(0);
	}

	int epfd;
	struct epoll_event ev;
	epfd = epoll_create(1);//传参正整数即可,无意义
	if (epfd < 0) {
		perror("epoll_create():");
		exit(1);
	}

	ev.events |= EPOLLIN;
	ev.data.fd = pd[0];
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, pd[0], &ev);
	if (ret==-1) {
		perror("epoll_ctl");
		exit(1);
	}
}//epoll_pipe


int main(int argc, const char *argv[])
{
	//epoll_pipe();
	epoll_normal_fd();
	return 0;
}
