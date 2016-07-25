#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "anytimer.h"
void f1(void *p)
{
	//sleep(10);
	puts(p);
}
void f2(void *p)
{
    puts(p);
}

void v1_test()
{
	//Begin!End!..bbb...aaa..ccc........
	int job1,job2,job3;
	int ret, i;

	puts("Begin!");
	job1 = at_addjob(5, f1, "aaa", NO_REPEAT);
	job2 = at_addjob(2, f1, "bbb", NO_REPEAT);
	job3 = at_addjob(7, f1, "ccc", NO_REPEAT);
	puts("End!");

	//版本1:
	//如果不sleep就是忙等
	//方便看效果
	while (1) {
		write(1, ".", 1);
		sleep(1);
	}

	//版本2:非忙等pause()版
	//while (1) {
	//	pause();
	//}
}

void v2_test()
{
	int job1,job2,job3;
	int ret, i;

	puts("Begin!");
	job1 = at_addjob(5, f1, "aaa", NO_REPEAT);
	job2 = at_addjob(2, f1, "bbb", REPEAT);
	job3 = at_addjob(7, f1, "ccc", NO_REPEAT);
	puts("End!");

	//版本3:
	//把pause()封装成wait来揭示多进程,多线程中wait这个阻塞的调用的内部机制就是pause等信号
	at_waitjob(job3);
	at_waitjob(job1);

	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "fail wait job2:%s\n", strerror(-ret));
	}

	ret = at_stopjob(job2);
	ret = at_canceljob(job2);
	ret = at_waitjob(job2);
	if (ret==0) {
		printf("success wait job2\n");
	}
}

/*
结果:
Begin!
End!
stop 3 seconds
***resume success
**aaa
bbb
**bbb
ccc
waitjob free id 2 
waitjob free id 0 
job2 is cron can not waitjob
**bbb
waitjob free id 1 
*/
/*
 分析1:
 停止job2,3秒后resume
 2秒后
 job1:aaa
 job2:bbb
 2秒
 job2:bbb
 job3:ccc
 2秒
 job2:bbb

 分析2:
 那个job先wait先回收那个job和时间无关
 */
void v3_test()
{
	int job1,job2,job3;
	int ret, i;

	//part1添加普通和周期任务
	puts("Begin!");
	job1 = at_addjob(5, f1, "aaa", NO_REPEAT);
	job2 = at_addjob(2, f1, "bbb", REPEAT);
	job3 = at_addjob(7, f1, "ccc", NO_REPEAT);
	puts("End!");

	//part2暂停/恢复
	ret = at_stopjob(job2);
	if (ret < 0) {
		printf("stop fail\n");
	}else {
		printf("stop 3 seconds\n");
		for (i = 0; i < 3; i++) {
			write(1, "*", 1);
			sleep(1);
		}
	}
	at_resumejob(job2);
	printf("resume success\n");
	

	//part3收尸 
	ret = at_waitjob(job3);//
	ret = at_waitjob(job1);//
	//周期性任务无法直接，要先cancel
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "job2 is cron can not waitjob\n");
	}
	//这样就可以回收了
	change_to_norepeat(job2);
	ret = at_canceljob(job2);
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "job2 is cron can not waitjob %s\n", strerror(-ret));
	}
}

//如果子任务执行时间过长 整个程序执行就会卡在那不动，子进程比较麻烦,觉得用脱离状态的线程
int main()
{
	//v1_test();
	//v2_test();
	v3_test();
	exit(0);
}
