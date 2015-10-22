#include <stdio.h>
#include <sched.h>
int main(int argc, const char *argv[])
{
	sched_yield();
	printf("111111111\n");
	sleep(123);
	return 0;
}
