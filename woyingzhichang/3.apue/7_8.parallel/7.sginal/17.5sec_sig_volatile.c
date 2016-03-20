#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

/*
volatile 优化flag 避免优化中的问题

gcc -O1 17.5sec_sig_volatile.c

time ./a.out 
10594468133
real	0m5.002s
user	0m5.004s
sys	0m0.000s
*/
//static volatile int loop=1;
static volatile sig_atomic_t loop=1;
void alarm_handler(int s){
	loop=0;
}

int main(int argc, const char *argv[])
{
	int64_t a=1;
	signal(SIGALRM, alarm_handler);
	alarm(5);
	while (loop) {
		a++;
	}
	printf("%lld\n", a);
	exit(0);
}
