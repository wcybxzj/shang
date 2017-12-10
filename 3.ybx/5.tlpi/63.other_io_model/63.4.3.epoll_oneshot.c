#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>

#define MAX_BUF     10
#define MAX_EVENTS     5

struct epoll_arr_st{
	int epfd;
	int fd_arrs[MAX_EVENTS];
	int arrs_num;
};

struct epoll_arr_st epoll_arr={
	.epfd=0,
	.fd_arrs={0,},
	.arrs_num=0
};

void errExit(char *str){
	perror(str);
	exit(1);
}

void reset_oneshot( int epollfd, int fd )
{
	struct epoll_event event;
	event.data.fd = fd;
	//event.events = EPOLLIN | EPOLLONESHOT;
	event.events = EPOLLIN | EPOLLONESHOT |EPOLLET;
	epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}

void handler(int s){
	int i;
	for (i = 0; i < epoll_arr.arrs_num; i++) {
		reset_oneshot(epoll_arr.epfd, epoll_arr.fd_arrs[i]);
	}
}

//测试1:epoll使用oneshot模式,fd只能识别一次就绪态,
//fd那边以后任何事件都不识别(包括ctrl+D)
/*
   终端1:
   [root@web11 63.other_io_model]# ./epoll_et_vs_lt_oneshot 123
   Opened "123" on fd 4
   About to epoll_wait()
Ready: 1
fd=4; events: EPOLLIN 
read 10 bytes: 1234567890
About to epoll_wait()
^C
[root@web11 63.other_io_model]#

终端2:
ot@web11 63.other_io_model]# cat > 123
1234567890abcdefhigk回车
ctrl+D
[root@web11 63.other_io_model]#
*/

//===============================================================

//测试2:(LT或者ET模式效果一样),资料来自hplsp9.3.4 EPOLLONESHOT事件
//当2个设置为oneshot的fd都从epoll_wait相应过一次
//ctrl+c 2个fd进行reset_one, 所有fd立刻可以将可以再从epoll_wait相应一次
/*
   终端1:
   ./63.4.3.epoll_oneshot 123 456
   Opened "123" on fd 4
   Opened "456" on fd 5
   About to epoll_wait()
Ready: 1
fd=4; events: EPOLLIN 
read 5 bytes: 1111

About to epoll_wait()
Ready: 1
fd=5; events: EPOLLIN 
read 6 bytes: 22222

About to epoll_wait()
^CEINTR -----------------------------ctrl+c
About to epoll_wait()
Ready: 2
fd=4; events: EPOLLIN 
read 5 bytes: 3333

fd=5; events: EPOLLIN 
read 5 bytes: 4444

About to epoll_wait()
^\Quit (core dumped)

终端2:
cat >123
1111
3333

终端3:
cat  > 456
22222
4444
*/
void epolloneshot(int argc, char * argv[]){
	int epfd, ready, fd, s, j, numOpenFds;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];
	char buf[MAX_BUF];
	if (argc < 2 || strcmp(argv[1], "--help") == 0){
		printf("%s file...\n", argv[0]);
		exit(1);
	}

	epfd = epoll_create(argc - 1);
	if (epfd == -1)
		errExit("epoll_create");
	epoll_arr.epfd = epfd;
	for (j = 1; j < argc; j++) {
		fd = open(argv[j], O_RDONLY);
		if (fd == -1)
			errExit("open");
		epoll_arr.fd_arrs[j-1] = fd;
		epoll_arr.arrs_num++;
		printf("Opened \"%s\" on fd %d\n", argv[j], fd);
		//ev.events = EPOLLIN|EPOLLONESHOT;
		ev.events = EPOLLIN|EPOLLONESHOT|EPOLLET;
		ev.data.fd = fd;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
			errExit("epoll_ctl");
	}
	numOpenFds = argc - 1;
	signal(SIGINT, handler);

	while (numOpenFds > 0) {
		printf("About to epoll_wait()\n");
		ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
		if (ready == -1) {
			if (errno == EINTR){
				printf("signal EINTR\n");
				continue;
			}
			else
				errExit("epoll_wait");
		}

		printf("Ready: %d\n", ready);

		for (j = 0; j < ready; j++) {
			printf("  fd=%d; events: %s%s%s\n", evlist[j].data.fd,
					(evlist[j].events & EPOLLIN)  ? "EPOLLIN "  : "",
					(evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "",
					(evlist[j].events & EPOLLERR) ? "EPOLLERR " : "");

			if (evlist[j].events & EPOLLIN) {
				s = read(evlist[j].data.fd, buf, MAX_BUF);
				if (s == -1)
					errExit("read");
				printf("    read %d bytes: %.*s\n", s, s, buf);

			} else if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
				if (evlist[j].events & EPOLLHUP) {
					printf("EPOLLHUP!!!\n");
				}
				if (evlist[j].events &  EPOLLERR) {
					printf("EPOLLERR!!!\n");
				}
				printf("    closing fd %d\n", evlist[j].data.fd);
				if (close(evlist[j].data.fd) == -1)
					errExit("close");
				numOpenFds--;
			}
		}
	}
	printf("All file descriptors closed; bye\n");
	exit(EXIT_SUCCESS);
}

//mkfifo 123 456
//tlpi表63-8
//tlpi63.4.6
int main(int argc, char *argv[])
{
	epolloneshot(argc, argv);
}
