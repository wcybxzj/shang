/* count_time.c */
#include <stdio.h>
#include <time.h>

/*
[root@web11 7.sginal]# time ./15.5sec_time
152278090

real	0m4.768s
user	0m4.745s
sys	0m0.015s
[root@web11 7.sginal]# time ./16.5sec_alarm
2320051597

real	0m5.013s
user	0m4.997s
sys	0m0.014s
*/

int main (void)
{
    long long count = 0;
    time_t t;

    t = time(NULL) + 5;

    while (time(NULL) < t) {
	        count++;
	    }

    printf("%lld\n", count);

    return 0;
}
