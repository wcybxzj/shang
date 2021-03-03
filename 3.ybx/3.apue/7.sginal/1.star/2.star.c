#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

void int_handler(int s){
	write(1, "!",1);
}

//一直按Ctrl+C,整个本来计划执行20秒的程序1秒就执行完
//程序要想终止 只能Ctrl+\ 终止程序,CTRL+C的默认动作被修改了

//观察1:write没有被中断而是sleep被中断如果补救
//观察write的返回值没问题1,sleep返回值是大于0

//观察2:一直ctrl+c程序一直在sleep, sleep被信号中断后,

int main(int argc, const char *argv[])
{
	struct timeval start, finish;

	int i, s;
    struct timespec request, remain;

    request.tv_sec = 1;
    request.tv_nsec = 0;

	signal(SIGINT, int_handler);

	for (i = 0; i < 10; i++) {
		s = write(1, "*", 1);
		if (s==-1) {
			printf("write()\n");
			return 0;
		}
		
		if (gettimeofday(&start, NULL) == -1){
			printf("gettimeofday()\n");
			return 0;
		}
		
		for (;;) {
			s = nanosleep(&request, &remain);
			if (s == -1 && errno != EINTR){
				printf("nanosleep()\n");
				return 0;
			}

			if (gettimeofday(&finish, NULL) == -1)
			{
				printf("gettimeofday()\n");
				return 0;
			}
				
			//printf("Slept for: %9.6f secs\n", finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0);

			if (s == 0)
				break;                      /* nanosleep() completed */

			//printf("Remaining: %2ld.%09ld\n", (long) remain.tv_sec, remain.tv_nsec);
			request = remain;               /* Next sleep is with remaining time */
		}

	}
	return 0;
}
