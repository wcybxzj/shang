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
int running_fd[MAX_EVENTS]={0,};

void errExit(char *str);
void handler(int s);
int get_pos(int *fdarr);
int is_empty(int *fdarr);
int is_full(int *fdarr);
int get_len(int *fdarr);
int add_fd(int *fdarr, int fd);
int delete_fd(int *fdarr,int fd);
void display(int *fd_arr);

void errExit(char *str){
	printf("errExit():");
	perror(str);
	exit(1);
}

void handler(int s){
	int j, len;
	char buf[MAX_BUF];
	len = get_len(running_fd);
	for (j = 0; j < len; j++) {
		s = read(running_fd[j], buf, MAX_BUF);
		if (s == -1){
			if (errno == EAGAIN) {
				delete_fd(running_fd, running_fd[j]);
			}else{
				errExit("read");
			}
		}else if(s==0){
			printf("closing fd %d\n",running_fd[j]);
			if (close(running_fd[j]) == -1)
				errExit("close");
			numOpenFds--;
			break;
		}else{
			printf("read %d bytes: %.*s\n", s, s, buf);
		}
	}
}

int get_pos(int *fdarr){
	int pos;
	if (is_full(fdarr)) {
		return -1;
	}
	pos = get_len(fdarr);
	return pos;
}

int is_empty(int *fdarr){
	if (get_len(fdarr)==0) {
		return 1;
	}else{
		return 0;
	}
}

int is_full(int *fdarr){
	if (get_len(fdarr)==MAX_EVENTS) {
		return 1;
	}else{
		return 0;
	}
}

int get_len(int *fdarr){
	int i, len = 0;
	for (i = 0; i < MAX_EVENTS; i++) {
		if (fdarr[i]!=0) {
			len++;
		}
	}
	return len;
}

int add_fd(int *fdarr, int fd){
	int pos;
	if (is_full(fdarr)) {
		return -1;
	}
	pos = get_pos(fdarr);
	fdarr[pos] = fd;
	return pos;
}

int delete_fd(int *fdarr,int fd){
	int len, i;
	if (is_empty(fdarr)) {
		return -1;
	}
	len = get_len(fdarr);
	for (i = 0; i < len; i++) {
		if (fdarr[i]==fd) {
			return i;
		}
	}
	return -1;
}

void display(int *fd_arr){
	int len, i;
	len = get_len(fd_arr);
	for (i = 0; i < len; i++) {
		printf("i:%d val:%d\n", i, fd_arr[i]);
	}
}

/*
终端1:
mkfifo 123 456
for i in {1..10}; do echo 11111111112222222222 > 123; done

终端2:
for i in {1..10}; do echo 44444444445555555555 > 456; done

终端3:
./63.4.6.epoll_et4  123 456
*/

//解决方式3:成功
//解决出现一个fd一直有数据其他fd有数据也没机会读取的饥饿现象
//边缘触发模式+非阻塞IO+epoll_wait超时返回0，发送SIGUSR1信号，在信号处理中对所有正在监控fd进行读取
void epollin_et_func4(int argc, char * argv[]){
	int i, fd_save, ret;
	int epfd, ready, fd, s, j;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];
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

	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}

	numOpenFds = argc - 1;
	while (numOpenFds > 0) {
		printf("About to epoll_wait()\n");
		ready = epoll_wait(epfd, evlist, MAX_EVENTS, 1*1000);//1秒
		if (ready == -1) {
			if (errno == EINTR){
				printf("EINTR\n");
				continue;
			}
			else
				errExit("epoll_wait");
		}

		printf("Ready: %d\n", ready);
		if (ready==0) {
			raise(SIGUSR1);
		}

		for (j = 0; j < ready; j++) {
			printf("  fd=%d; events: %s%s%s\n", evlist[j].data.fd,
					(evlist[j].events & EPOLLIN)  ? "EPOLLIN "  : "",
					(evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "",
					(evlist[j].events & EPOLLERR) ? "EPOLLERR " : "");
			if (evlist[j].events & EPOLLIN) {
				add_fd(running_fd, evlist[j].data.fd);
				if (ret==-1) {
					printf("add_fd error\n");
					exit(1);
				}
			} else if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
				ret = delete_fd(running_fd, evlist[j].data.fd);
				if (ret==-1) {
					printf("delete_fd error\n");
					exit(1);
				}
				numOpenFds--;
			}else{
				printf("11111111111111\n");
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
	epollin_et_func4(argc, argv);

	//数据结构测试
	//int len;
	//int running_fd[MAX_EVENTS]={0,};
	//add_fd(running_fd, 123);
	//add_fd(running_fd, 456);
	//display(running_fd);
}
