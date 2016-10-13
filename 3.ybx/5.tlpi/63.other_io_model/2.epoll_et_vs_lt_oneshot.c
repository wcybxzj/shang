#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_BUF     10
#define MAX_EVENTS     5
                        
void errExit(char *str){
	perror(str);
	exit(1);
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
^C
[root@web11 63.other_io_model]#

终端2:
ot@web11 63.other_io_model]# cat > 123
1234567890abcdefhigk回车
ctrl+D
[root@web11 63.other_io_model]#
*/
//epoll使用oneshot模式,fd只能识别一次就绪态, fd那边以后任何事件都不识别(包括ctrl+D)
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

    for (j = 1; j < argc; j++) {
        fd = open(argv[j], O_RDONLY);
        if (fd == -1)
            errExit("open");
        printf("Opened \"%s\" on fd %d\n", argv[j], fd);

        //ev.events = EPOLLIN;
        ev.events = EPOLLIN|EPOLLONESHOT;
        //ev.events = EPOLLIN|EPOLLET;
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
./epoll_et_vs_lt_oneshot 123
Opened "123" on fd 4
About to epoll_wait()
	Ready: 1
	  fd=4; events: EPOLLIN 
	      read 10 bytes: 1234567890
		  About to epoll_wait()
	Ready: 1
	  fd=4; events: EPOLLIN 
	      read 10 bytes: abcdefhigk
		  About to epoll_wait()
	Ready: 1
	  fd=4; events: EPOLLIN 
	      read 1 bytes: 

		  About to epoll_wait()
	Ready: 1
	  fd=4; events: EPOLLHUP 
	  EPOLLHUP!!!
	      closing fd 4
		  All file descriptors closed; bye

终端2:
[root@web11 63.other_io_model]# cat > 123
1234567890abcdefhigk回车
Ctrl+D
*/
//epollin默认是水平触发+阻塞IO,
//输入的数据是20字节，read 缓存是10字节
//只要有数据epoll_wait就会解除阻塞
void epollin_lt(int argc, char * argv[]){
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

        ev.events = EPOLLIN;
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
//结果:实际没解决问题应,一次epoll_wait后read一直阻塞
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


//解决方式2:io nonblock,来读取读到没内容EAGAIN  continue
//结果:完美解决
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
					}
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

//mkfifo 123 456
//tlpi表63-8
//tlpi63.4.6
int main(int argc, char *argv[])
{
	//epolloneshot(argc, argv);
	//epollin_lt(argc, argv);
	//epollin_et(argc, argv);
	//epollin_et_func1(argc, argv);
	epollin_et_func2(argc, argv);
}
