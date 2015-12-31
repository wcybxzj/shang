#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void alrm_handler(int s)
{
	alarm(1);
	printf("Got SIGALRM\n");
}

int main()
{

	signal(SIGALRM,alrm_handler);
	alarm(3);
//	alarm(5);
//	alarm(10);
//	alarm(1);

	while(1)
	{
		printf("while(1);\n");
		sleep(1);
	}

	exit(0);
}


