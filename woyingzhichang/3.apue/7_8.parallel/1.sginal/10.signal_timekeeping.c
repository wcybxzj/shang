#include <stdio.h>
#include <signal.h>
#include <time.h>
time_t start;
time_t end;

void u1_handler(int s)
{
	start = time(NULL);
}

void u2_handler(int s)
{
	printf("111\n");
	end = time(NULL);
}

void func()
{
	raise(SIGUSR1);
	int i=0;
	for (i = 0; i < 3; i++) {
		sleep(1);
	}
	raise(SIGUSR2);
	printf("222\n");
}

//使用信号做一个计时器
int main(int argc, const char *argv[])
{
	signal(SIGUSR1, u1_handler);
	signal(SIGUSR2, u2_handler);
	func();
	printf("%ld\n", start);
	printf("%ld\n", end);
	return 0;
}
