#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void alrm_handler(int s)
{
	alarm(1);
	printf("Got SIGALRM\n");
	return ;
}

int main()
{

	signal(SIGALRM,alrm_handler);
	alarm(3);

	while(1)
	{
		printf("while(1)\n");
		sleep(1);
	}

	exit(0);
}
