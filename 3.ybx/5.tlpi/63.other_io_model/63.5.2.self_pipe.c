#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <strings.h>

#define MAX 1024
#define BUF_SIZE 1024
static int pfd[2];

static void handler(int s){
	int ret;
	ret = write(pfd[1],"x",1);
	printf("handler ret:%d\n", ret);
	if (ret == -1) {
		if (errno != EAGAIN) {
			perror("write");
			exit(1);
		}
	}
}

int max(int a, int b){
	if (a>b) {
		return a;
	}else{
		return b;
	}
}



/*
 终端1:
./63.5.2.self_pipe 123 456
pid:14321
block open
unblock open
before select
after select
ready:1
normal data
that was two
before select
^Chandler ret:1
after select
ready:-1
before select

终端2:
cat > 123
that was two

终端3:
cat > 456
*/

//通过self-pipe,让信号进入管道,用select监控管道fd
//实现使用IO复用借口,同时监控普通文件事件和信号
//mkfio 123 456
//./a.out 123 456
int main(int argc, const char *argv[])
{
	int nfds, i, fd1, fd2, flags;
	int ready, num;
	fd_set readfds;
	char buf[BUF_SIZE];
	struct sigaction sa;
	char ch;
	if (argc != 3) {
		printf("lack argv\n");
		exit(1);
	}

	printf("pid:%d\n", getpid());

	printf("block open\n");
	nfds = 0;
	FD_ZERO(&readfds);
	fd1 = open(argv[1], O_RDONLY);
	fd2 = open(argv[2], O_RDONLY);
	FD_SET(fd1, &readfds);
	FD_SET(fd2, &readfds);
	printf("unblock open\n");

	//pipe
	if (pipe(pfd) == -1) {
		perror("pipe");
		exit(1);
	}
	nfds = max(nfds, pfd[0]+1);
	FD_SET(pfd[0], &readfds);

	//nonblock pipe
	flags = fcntl(pfd[0], F_GETFL);
	if (flags == -1) {
		perror("fcntl()");
		exit(1);
	}
	flags |= O_NONBLOCK;
	if (fcntl(pfd[0], F_SETFL, flags) == -1) {
		perror("fcntl()");
		exit(1);
	}

	flags = fcntl(pfd[1], F_GETFL);
	if (flags == -1) {
		perror("fcntl()");
		exit(1);
	}
	flags |= O_NONBLOCK;
	if (fcntl(pfd[1], F_SETFL, flags) == -1) {
		perror("fcntl()");
		exit(1);
	}

	//sigaction
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;// for read
	if (sigaction(SIGINT, &sa, 0)) {
		perror("sigaction()");
		exit(1);
	}

	//select
	while (1) {
		printf("before select\n");
		ready = select(nfds, &readfds, NULL, NULL, NULL);
		printf("after select\n");
		printf("ready:%d\n", ready);

		if (ready == -1) {
			if (errno == EINTR) {
				continue;
			}else{
				perror("select");
				exit(1);
			}
		}else{
			if (FD_ISSET(pfd[0], &readfds)) {
				printf("A signal is catch!\n");
				while (1) {
					num = read(pfd[0], &ch, 1);
					if (num == -1) {
						if (errno == EAGAIN) {
							break;//管道读端非阻塞的用处
						}else{
							perror("read");
							exit(1);
						}
					}
				}
			}else{
				bzero(buf, BUF_SIZE);
				printf("normal data\n");
				if (FD_ISSET(fd1, &readfds)) {
					num = read(fd1, buf, BUF_SIZE);
					if (num ==-1) {
						perror("read");
						exit(1);
					}else{
						printf("%s",buf);
						fflush(NULL);
					}
				}else if (FD_ISSET(fd2, &readfds)) {
					num = read(fd2, buf, BUF_SIZE);
					if (num ==-1) {
						perror("read");
						exit(1);
					}else{
						printf("%s",buf);
						fflush(NULL);
					}
				}
			}
		}
	}
	return 0;
}
