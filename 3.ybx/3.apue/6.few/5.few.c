#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, const char *argv[])
{
	pid_t pid;
	printf("BEGIN\n");
	fflush(NULL);
	pid = fork();
	if (pid<0) {
		perror("fork()");
		exit(0);
	}

	if (pid == 0) {
		execl("/bin/date", "www", "+%s",NULL);
		perror("execl():");
		exit(1);
	}

	wait(NULL);
	printf("END\n");
	exit(0);
}
