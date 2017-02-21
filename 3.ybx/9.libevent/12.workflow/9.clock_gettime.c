//函数的原型如下：
//int clock_gettime(clockid_t clk_id, struct timespec *tp);
//
//clockid_t clk_id用于指定计时时钟的类型:
//CLOCK_PROCESS_CPUTIME_ID, high-resolution timer provided by the CPU for each process.
//CLOCK_THREAD_CPUTIME_ID, high-resolution timer provided by the CPU for each of the threads.
//CLOCK_REALTIME： Systemwide realtime clock. 系统范围内的实时时钟。
//CLOCK_MONOTONIC：Represents monotonic time. Cannot be set. 表示单调时间，不能被设置的。
//
//struct timespec {
//    time_t   tv_sec;        /* seconds */
//    long     tv_nsec;       /* nanoseconds */
//};
//
//在编译链接时需加上 -lrt ;因为在librt中实现了clock_gettime函数。
//struct timespec ts;
//clock_gettime(CLOCK_MONOTONIC,ts);
//
//CLOCK_REALTIME：
//当系统的时钟源被改变，这种类型的时钟可以
//得到相应的调整，也就是说，系统时间影响这种类型的timer。
//CLOCK_MONOTONIC：
//用的是相对时间，他的时间是通过jiffies值来计算的。
//该时钟不受系统时钟源的影响，只受jiffies值的影响。
//
//建议使用：
//CLOCK_MONOTONIC这种时钟更加稳定，不受系统时钟的影响。
//如果想反映wall clock time，就使用CLOCK_REALTIME。
//
//clock_gettime比gettimeofday更加精确
//clock_gettime() 提供了纳秒的精确度，给程序计时可是不错哦； 



#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc,char *argv[])
{
	struct timespec time1 = {0, 0};
	clock_gettime(CLOCK_REALTIME, &time1);
	printf("CLOCK_REALTIME: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
	clock_gettime(CLOCK_MONOTONIC, &time1);
	printf("CLOCK_MONOTONIC: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);//开机到现在8小时
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
	printf("CLOCK_PROCESS_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
	printf("CLOCK_THREAD_CPUTIME_ID: %lu, %lu\n", time1.tv_sec, time1.tv_nsec);
	printf("\n%lu\n", time(NULL));
	sleep(5);//为了进程和线程时间
	return 0;
}
