#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
   终端1: ./wait &

   终端2:
   ybx@x220:~/c_www/shangguan/woyingzhichang/3.apue/7.parallel/1.signal/4.SIGCONT$ ps -axf|grep wait
   4691 pts/8    S      0:00          |   |   \_ ./wait
   4692 pts/8    S      0:00          |   |       \_ ./wait
   4696 pts/11   S+     0:00          |   |   \_ grep --color=auto wait
   4604 pts/12   S+     0:00          |       \_ man wait
   ybx@x220:~/c_www/shangguan/woyingzhichang/3.apue/7.parallel/1.signal/4.SIGCONT$ kill -STOP 4692
   ybx@x220:~/c_www/shangguan/woyingzhichang/3.apue/7.parallel/1.signal/4.SIGCONT$ ps -axf|grep wait
   4691 pts/8    S      0:00          |   |   \_ ./wait
   4692 pts/8    T      0:00          |   |       \_ ./wait
   4698 pts/11   S+     0:00          |   |   \_ grep --color=auto wait
   4604 pts/12   S+     0:00          |       \_ man wait
   ybx@x220:~/c_www/shangguan/woyingzhichang/3.apue/7.parallel/1.signal/4.SIGCONT$ kill -CONT 4692
   ybx@x220:~/c_www/shangguan/woyingzhichang/3.apue/7.parallel/1.signal/4.SIGCONT$ ps -axf|grep wait
   4691 pts/8    S      0:00          |   |   \_ ./wait
   4692 pts/8    S      0:00          |   |       \_ ./wait
   4700 pts/11   S+     0:00          |   |   \_ grep --color=auto wait
   4604 pts/12   S+     0:00          |       \_ man wait
*/


//前台运行 
//停止和启动子进程:(kill -TSTP 子进程id/ kill -CONT 子进程id)
//后台运行 
//停止和启动子进程:(kill -STOP 子进程id/ kill -CONT 子进程id)
int main(int argc, const char *argv[])
{
	int i;
	int status;
	pid_t pid;

	pid = fork();
	if (pid==0) {
		for (i = 0; ; i++) {
			printf("%d\n", i);
			sleep(1);
		}
		exit(1);
	}

	printf("父:%d 子:%d\n", getpid(), pid);
	wait(status);
	printf("父进程wait status:%d\n", status);
	if (WIFCONTINUED(status)) {
		printf("子进程 CONTINUE\n");
	}
	if (WIFSTOPPED(status)) {
		printf("子进程 CONTINUE\n");
	}
	return 0;
}
