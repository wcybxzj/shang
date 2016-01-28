#include <stdio.h>

#include <linux/time.h>
#include<sys/times.h>
#include <unistd.h>
#define CLOCKS_PER_SEC 1l


int main()
{
	struct tms tmp;
	struct tms tmp1, tmp2;
	clock_t times_1, times_2;
	clock_t begin = times(&tmp);
	times_1 = times(&tmp1);
	/* your code */
	sleep(2);
	times_2 = times(&tmp2);

	printf("==times tms_utime HandControlProcess=== %lf\n", (double)(tmp2.tms_utime-tmp1.tms_utime)/1);
	printf("==times tms_stime HandControlProcess=== %lf\n", (double)(tmp2.tms_stime-tmp1.tms_stime)/1);
	printf("==times tms_cutime HandControlProcess=== %lf\n", (double)(tmp2.tms_cutime-tmp1.tms_cutime)/1);
	printf("==times tms_cstime HandControlProcess=== %lf\n", (double)(tmp2.tms_cstime-tmp1.tms_cstime)/1);
	printf("==times tms_cstime HandControlProcess times_2-times_1=== %lf\n", (double)(times_2-times_1)/1);

	clock_t end = times(&tmp);
	unsigned long HZ = sysconf( _SC_CLK_TCK );

	printf("HZ=%ld\n", HZ);
	printf("%lf\n", (double)(end-begin)/HZ );


}
