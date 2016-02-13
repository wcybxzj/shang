#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

/*
time ./16.5sec_sig 
1615179793

real	0m5.002s
user	0m5.004s
sys	0m0.000s
*/
int loop=1;
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
