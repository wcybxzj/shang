#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define LEFT  30000000
#define RIGHT 30000200

#define NUM 3

int main(int argc, const char *argv[])
{
	pid_t pid;
	int i, j, n, mark;

	for (n = 0; n < NUM; n++) {
		pid = fork();
		//printf("gtepid() is %d, n  is%d\n", getpid(), n);
		if (pid < 0) {
			for (i = 0; i < n; i++) {//严谨的回收之前创建的子进程
				wait(NULL);
			}
			perror("fork()");
			exit(1);
		}

		if (pid == 0) {
			for (i = LEFT+n; i <= RIGHT ; i=i+NUM) {
				mark = 1;
				for (j = 2; j < i/2 ; j++) {
					if (i % j == 0) {
						mark = 0;
						break;
					}
				}
				if (mark) {
					printf("[%d]pid is %d, %d is primer\n",n ,getpid(), i);//可以看到0号一直没打印
				}
			}
			exit(0);
		}

	}


	for (i = 0; i <= NUM; i++) {
		wait(NULL);
	}
	return 0;
}
