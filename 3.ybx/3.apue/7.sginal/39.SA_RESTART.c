#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
//证明SA_RESTART的用处
//慢系统调用read被信号打断。 如果设置了SA_RESTART read即使被打断可以再次复位

void int_handler (int signum)
{
	printf ("int handler %d\n",signum);
}


/*
./39.SA_RESTART
可以输入了
^Cint handler 2
hahahah
read 8 bytes, content is hahahah
可以输入了
*/
void use_SA_RESTART()
{
	char buf[100];
	ssize_t ret;
	struct sigaction oldact;
	struct sigaction act;

	act.sa_handler = int_handler;
	act.sa_flags=0;
	act.sa_flags |= SA_RESTART;
	sigemptyset(&act.sa_mask);
	if (-1 == sigaction(SIGINT,&act,&oldact))
	{
		printf("sigaction failed!\n");
		exit(1);
	}

	bzero(buf,100);
	while (1) {
		printf("可以输入了\n");
		ret = read(STDIN_FILENO,buf,10);
		if (ret == -1)
		{
			printf ("read error %s\n", strerror(errno));
			exit(1);
		}else{
			printf ("read %d bytes, content is %s",ret,buf);
		}
	}
}

/*
可以输入了
^Cint handler 2
read error Interrupted system call
*/
void not_use_SA_RESTART()
{
	char buf[100];
	ssize_t ret;
	struct sigaction oldact;
	struct sigaction act;

	act.sa_handler = int_handler;
	act.sa_flags=0;
	if (-1 == sigaction(SIGINT,&act,&oldact))
	{
		printf("sigaction failed!\n");
		exit(1);
	}

	bzero(buf,100);
	while (1) {
		printf("可以输入了\n");
		ret = read(STDIN_FILENO,buf,10);
		if (ret == -1)
		{
			printf ("read error %s\n", strerror(errno));
			exit(1);
		}else{
			printf ("read %d bytes, content is %s",ret,buf);
		}
	}
}

int main(int argc, char **argv)
{
	use_SA_RESTART();
	//not_use_SA_RESTART();
	return 0;
}

