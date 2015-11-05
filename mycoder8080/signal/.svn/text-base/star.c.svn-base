#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void int_handler(int s)
{
	write(1,"!",1);
}

int main()
{
	int i;
	struct sigaction sa;

//	signal(SIGINT,SIG_IGN);
//	signal(SIGINT,int_handler);

	sa.sa_handler = int_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGINT,&sa,NULL) < 0)
	{
		perror("sigaction()");
		exit(1);
	}


	for(i = 0 ; i < 10 ; i++)
	{
		write(1,"*",1);
		sleep(1);
	}

	exit(0);
}


