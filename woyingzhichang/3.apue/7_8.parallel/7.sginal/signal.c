#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


static void daemon_exit(int s)
{
	int i;
	printf("start signal:%d\n", s);
	for (i = 0; i < 10; i++) {
		printf("signal:%d , i:%d\n",s , i);
		sleep(1);
	}
	printf("end signal:%d\n", s);
}

int main()
{
	int i;
	struct sigaction sa;

	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGINT);
	sa.sa_flags = 0;
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGQUIT,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);

	while (1) {
		pause();
	}

	exit(0);
}


