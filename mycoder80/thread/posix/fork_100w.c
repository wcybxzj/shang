#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define NUM 1000000
int main(int argc, const char *argv[])
{
	int i;
	pid_t pid;
	time_t begin;
	begin = time(NULL);
	printf("%ld\n", begin);
	for (i = 0; i < NUM; i++) {
		pid = fork();
		if (pid > 0) {

		}
		if (pid==0) {
			exit(1);
		}
	}
	time_t end = time(NULL);
	printf("%ld\n", end);
	for (i = 0; i < NUM; i++) {
		wait(NULL);
	}

	return 0;
}
