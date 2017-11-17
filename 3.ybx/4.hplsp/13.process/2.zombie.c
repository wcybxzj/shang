#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

//第二种僵尸情况(hplsp page240):
//父进程先结束，子进程结束时候没父亲被init收养回收
//在父结束后，子结束前，子进程处于僵尸态(根本不对)
/*
watch -n1 'ps axf|grep zombie'
*/
int main(int argc, const char *argv[])
{
	pid_t pid;
	int i;
	pid = fork();
	if (pid==0) {
		while (1) {
			i++;
			if (i>100000000) {
				break;
			}
			sleep(1);
		}
		printf("child pid:%d\n", getpid());
		exit(1);
	}

	printf("parent pid:%d\n", getpid());
	return 0;
}
