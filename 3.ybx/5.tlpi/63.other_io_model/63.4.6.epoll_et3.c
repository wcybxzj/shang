#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define MAX_BUF     10
#define MAX_EVENTS     5
int numOpenFds;


void errExit(char *str){
	perror(str);
	exit(1);
}

/*
基本解决的例子
终端1:
[root@web11 63.other_io_model]# ./epoll_et_vs_lt_oneshot 123 456
Opened "123" on fd 4
Opened "456" on fd 5
About to epoll_wait()
Ready: 1
  fd=4; events: EPOLLIN 
      read 10 bytes: 1234567890
	  read 10 bytes: abcdefghij
	  read 2 bytes: k
	  About to epoll_wait()
Ready: 1
  fd=5; events: EPOLLIN 
      read 10 bytes: 444444444
	  About to epoll_wait()
Ready: 1
  fd=5; events: EPOLLHUP 
  EPOLLHUP!!!
      closing fd 5
	  About to epoll_wait()
Ready: 1
  fd=4; events: EPOLLHUP 
  EPOLLHUP!!!
      closing fd 4
	  All file descriptors closed; bye]

终端2:
mkfifo 123
mkfifo 456
[root@web11 63.other_io_model]# cat > 123
1234567890abcdefghijk回车
ctrl+d
[root@web11 63.other_io_model]#

终端3:
[root@web11 63.other_io_model]# cat > 456
444444444回车
ctrl+d
[root@web11 63.other_io_model]#
*/

/*
例子文件描述符饥饿现象
[./63.4.6.epoll_et_vs_lt_oneshot 123 456
Opened "123" on fd 4
Opened "456" on fd 5
About to epoll_wait()
Ready: 2
  fd=4; events: EPOLLIN EPOLLHUP 
    read 10 bytes: 1111111111
    read 10 bytes: 2222222222
    read 10 bytes: 1111111122
	.......................
    read 10 bytes: 22222222
    read 10 bytes: 1111111112
    read 10 bytes: 222222222

    closing fd 4
  fd=5; events: EPOLLIN EPOLLHUP 
    read 10 bytes: 4444444444
    read 10 bytes: 5555555555
	........................
    read 10 bytes: 4444444455
    read 10 bytes: 55555555
    read 10 bytes: 4444444445
    read 10 bytes: 555555555

    closing fd 5
All file descriptors closed; bye

终端2:
for i in {1..10}; do echo 11111111112222222222 > 123; done

终端3:
for i in {1..10}; do echo 44444444445555555555 > 456; done
*/

//解决方式2:基本成功
//边缘触发模式+非阻塞IO+read()循环多读几次,读取读到没内容EAGAIN  continue
//问题:但是如果一个fd一直有数据其他fd还是没机会运行,
//比如一个fd1一直有数据在readfd2, fd3就算来数据也没机会读
//(tlpi63.4.6当边缘触发通知时避免出现文件描述饥饿现象)
void epollin_et_func3(int argc, char * argv[]){
	int fd_save;
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

	for (j = 1; j < argc; j++) {
		fd = open(argv[j], O_RDONLY);
		fd_save = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, fd_save|O_NONBLOCK);
		if (fd == -1)
			errExit("open");
		printf("Opened \"%s\" on fd %d\n", argv[j], fd);

		ev.events = EPOLLIN|EPOLLET;
		ev.data.fd = fd;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
			errExit("epoll_ctl");
	}

	numOpenFds = argc - 1;

	while (numOpenFds > 0) {
		printf("About to epoll_wait()\n");
		ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
		if (ready == -1) {
			if (errno == EINTR){
				printf("EINTR\n");
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
				while (1) {
					s = read(evlist[j].data.fd, buf, MAX_BUF);
					if (s == -1){
						if (errno == EAGAIN) {
							printf("EAGAIN\n");
							break;
						}else{
							errExit("read");
						}
					}else if(s==0){
						printf("    closing fd %d\n", evlist[j].data.fd);
						if (close(evlist[j].data.fd) == -1)
							errExit("close");
						numOpenFds--;
						break;
					}else{
						printf("    read %d bytes: %.*s\n", s, s, buf);
					}
				}
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

//tlpi表63-8
//tlpi63.4.6
int main(int argc, char *argv[])
{
	epollin_et_func3(argc, argv);

	//数据结构测试
	//int len;
	//int running_fd[MAX_EVENTS]={0,};
	//add_fd(running_fd, 123);
	//add_fd(running_fd, 456);
	//display(running_fd);
}
