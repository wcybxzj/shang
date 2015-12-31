#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT		30000000
#define RIGHT		30000200
#define N			3	

int main()
{
	int i,n,j,mark;
	pid_t pid;
	
	for(n = 0 ; n < N ; n++)
	{
		pid = fork();		
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if(pid == 0)
		{
			for(i = LEFT+n ; i <= RIGHT; i+=N)
			{
				mark = 1;
				for(j = 2; j < i/2; j++)
				{
					if(i % j == 0)
					{
						mark = 0;
						break;
					}
				}
				if(mark)
					printf("[%d]%d is a primer.\n",n,i);
			}
			exit(0);
		}
	}

	for(n = 0 ; n < N ; n++)
		wait(NULL);

	exit(0);
}


