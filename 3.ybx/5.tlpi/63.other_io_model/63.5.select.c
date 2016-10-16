#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int got;

void handler(int sig, siginfo_t *info, void *ptr){
	got=1;
}

/*
失败的例子
终端1:
mkfifo 123
./63.5.select 123
pid:10491
open block
open unblock
sleep i:0, 在此发送信号select户不会被打断
...................
sleep i:9, 在此发送信号select户不会被打断
before select

终端2:信号发生在sleep0-9中
kill -USR1 8728
*/

/*
成功的例子
终端1:
./63.5.select 123
pid:11015
open block
open unblock
sleep i:0, 在此发送信号select户不会被打断
...................
sleep i:9, 在此发送信号select户不会被打断

before select
after select
got!
before select
after select
got!
before select

终端2:
kill -USR1 11015
kill -USR1 11015
*/

//目的:同时进行io possible和信号处理的问题
//现在这种方式的问题是信号发生在select前,select不会被信号打断
//解决方法1:pselect(linux特有)
//解决方法2:self-pipe(通用方法)
//解决方法3:signalfd(tlpi 22.1)
int main(int argc, const char *argv[])
{
	int i, num, fd;
	fd_set rset;
	struct sigaction sa;

	if (argc!=2) {
		printf("lack of argv\n");
		exit(1);
	}

	printf("pid:%d\n", getpid());
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction()");
		exit(1);
	}
	printf("open block\n");
	fd = open(argv[1], O_RDONLY);
	if (fd==-1) {
		perror("open()");
		exit(1);
	}
	printf("open unblock\n");

	for (i = 0; i < 10; i++) {
		printf("sleep i:%d, 在此发送信号select户不会被打断\n", i);
		sleep(1);
		printf("\n");
	}

	while (1) {
		printf("before select\n");
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		num = select(fd+1,&rset,NULL, NULL,NULL);
		printf("after select\n");
		if (num>0) {
			printf("select is ok\n");
		}else{
			if (errno == EINTR) {
				if (got) {
					printf("got!\n");
				}
			}else{
				perror("select");
			}
		}
	}
	return 0;
}
