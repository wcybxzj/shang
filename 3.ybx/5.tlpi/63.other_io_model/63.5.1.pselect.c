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

void handler(int sig){
	got=1;
}

/*
终端1:
./63.5.1.pselect 123
pid:11158
open block
open unblock
sleep i:0, 在此发送信号select户不会被打断
.........................................
sleep i:9, 在此发送信号select户不会被打断

before select
after select
got!
before select

终端2:
kill -USR1 11158
*/

//解决方法1:pselect(linux特有)
//信号即使发生在sleep 0-9也可以保证让信号去打断pselect
//pselect很像sigsupend
int main(int argc, const char *argv[])
{
	int i, num, fd;
	fd_set rset;
	struct sigaction sa;
	sigset_t emptyset, blockset;
	if (argc!=2) {
		printf("lack of argv\n");
		exit(1);
	}
	printf("pid:%d\n", getpid());

	sigemptyset(&blockset);
	sigaddset(&blockset, SIGUSR1);
	if (sigprocmask(SIG_BLOCK, &blockset, NULL) == -1) {
		perror("sigprocmask");
		exit(1);
	}

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
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
		sigemptyset(&emptyset);
		num = pselect(fd+1,&rset,NULL, NULL,NULL, &emptyset);
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
