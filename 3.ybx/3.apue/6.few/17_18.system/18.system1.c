#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

//模拟system的执行过程模拟
int main(int argc, const char *argv[])
{
	pid_t pid;
	pid = fork();
	if (pid < 0 ) {
		perror("fork():");
		exit(1);
	}

	if (pid == 0) {
		execl("/bin/sh", "sh", "-c", "date +%s", NULL);
		perror("execl");
		exit(1);
	}
	wait(NULL);

	return 0;
}
