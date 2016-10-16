#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_BUF     10
#define MAX_EVENTS     5
int numOpenFds;
int running_fd[MAX_EVENTS]={0,};

void errExit(char *str){
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
			printf("    closing fd %d\n",running_fd[j]);
			if (close(running_fd[j]) == -1)
				errExit("close");
			numOpenFds--;
			break;
		}else{
			printf("    read %d bytes: %.*s\n", s, s, buf);
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
[root@web11 63.other_io_model]# ./epoll_et_vs_lt_oneshot 123
Opened "123" on fd 4
About to epoll_wait()
Ready: 1
  fd=4; events: EPOLLIN 
      read 10 bytes: 1234567890
	  About to epoll_wait()
Ready: 1
  fd=4; events: EPOLLIN 
      read 10 bytes: abcdefghij
	  About to epoll_wait()
Ready: 1
  fd=4; events: EPOLLIN EPOLLHUP 
      read 10 bytes: k
	  lmnopqrs
	  About to epoll_wait()

终端2:
[[root@web11 63.other_io_model]# cat > 123
1234567890abcdefghijk回车
lmnopqrstu回车
ctrl+D
[root@web11 63.other_io_model]#]
*/
//边缘触发模式+阻塞IO:
//输入20字符,但是epoll_wait返回后read 一次只能读10字节,残留10字节此模式下第二次epoll_wait还是阻塞
//输入数据才会触发epoll_wait解除阻塞,fd中有数据可读epoll_wait仍然阻塞
void epollin_et(int argc, char * argv[]){
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

/*
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


终端2:
[root@web11 63.other_io_model]# cat > 123
1234567890abcdefghijk回车

终端3:
[root@web11 63.other_io_model]# cat > 456
44444444444444444444444
*/

//解决方式1:阻塞io 虽然read 1次10字节多读几次,最后阻塞卡在read
//结果:实际没解决问题,一次epoll_wait后read一直阻塞
//造成如果监控多个fd没机会再epoll_wait获取就绪态
void epollin_et_func1(int argc, char * argv[]){
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
					if (s == -1)
						errExit("read");
					printf("    read %d bytes: %.*s\n", s, s, buf);
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
//解决方式2:io nonblock,来读取读到没内容EAGAIN  continue
//结果:基本解决
//问题:但是如果一个fd一直有数据其他fd还是没机会运行,
//比如一个fd1一直有数据在read,fd2 fd3就算来数据也没机会读
//(tlpi63.4.6当边缘触发通知时避免出现文件描述饥饿现象)
void epollin_et_func2(int argc, char * argv[]){
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

//解决方式3:解决出现一个fd一直有数据其他fd有数据也没机会读取的饥饿现象
void epollin_et_func3(int argc, char * argv[]){
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
			}
		}

	}
	printf("All file descriptors closed; bye\n");
	exit(EXIT_SUCCESS);
}

/*
 终端1:
	read 10 bytes: 1112222222
		 read 10 bytes: 222
		 111111
		 read 10 bytes: 4444444444
		 About to epoll_wait()
	Ready: 0
		read 10 bytes: 1111222222
			read 10 bytes: 2222
			11111
			read 10 bytes: 1111122222
			read 10 bytes: 4444444444
			About to epoll_wait()
	Ready: 0
		read 10 bytes: 22222
		1111
			read 10 bytes: 1111112222
			read 10 bytes: 222222
			111
			read 10 bytes: 
			444444444
			About to epoll_wait()

 终端2:
 for i in {1..10}; do echo 11111111112222222222 > 123; done

 终端3:
 for i in {1..10}; do echo 44444444444444444444 > 456; done
 */

//mkfifo 123 456
//tlpi表63-8
//tlpi63.4.6
int main(int argc, char *argv[])
{

	//epollin_et(argc, argv);
	//epollin_et_func1(argc, argv);
	//epollin_et_func2(argc, argv);
	epollin_et_func3(argc, argv);


	//数据结构测试
	//int len;
	//int running_fd[MAX_EVENTS]={0,};
	//add_fd(running_fd, 123);
	//add_fd(running_fd, 456);
	//display(running_fd);
}
