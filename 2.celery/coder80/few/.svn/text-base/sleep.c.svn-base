#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*sleep 100*/

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
		execl("/bin/sleep","httpd","100",NULL);
		perror("execl()");
		exit(1);
	}
	else
		wait(NULL);
	
	puts("End!");

	exit(0);
}



