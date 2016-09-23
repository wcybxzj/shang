/* count_alarm.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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


time版本时间4.7  count:152278090
alarm版本时间5.0 count:2320051597

表现是:alarm更精确，原因就是alarm更多次的去利用中断去内核去判断时间,
这就让count有更多的机会去自增
*/

long long count = 0;
static volatile int flag = 1;

void alarm_handler (int s)
{
    flag = 0;
}

int main (void)
{
    signal(SIGALRM, alarm_handler);
    alarm(5);

    flag = 1;

    while (flag)
    {
	        count++;
	}
    printf("%lld\n", count);
    return 0;
}
