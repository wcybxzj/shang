#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/*
time ./15.5sec_time 
1430945750
real	0m10.041s
user	0m10.040s
sys	0m0.008s
*/

int main(int argc, const char *argv[])
{
	time_t end;
	int64_t a=1;
	end = time(NULL)+5;
	while (time(NULL)<= end) {
		a++;
	}
	printf("%lld\n", a);
	exit(0);
}
