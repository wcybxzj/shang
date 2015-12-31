#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*date +%s*/

int main()
{
	pid_t pid;

	puts("Begin!");	

	fflush(NULL);/*!!!*/

	pid = fork();	
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	if(pid == 0)
	{
		execl("/bin/sh","sh","-c","date +%s",NULL);
		perror("execl()");
		exit(1);
	}
	else
		wait(NULL);
	
	puts("End!");

	exit(0);
}



