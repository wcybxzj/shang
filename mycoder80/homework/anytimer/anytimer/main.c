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
//如果子任务执行时间过长 整个程序执行就会卡在那不动，子进程比较麻烦,觉得用脱离状态的线程
int main()
{//Begin!End!..bbb...aaa..ccc........
	int job1,job2,job3;
	int ret, i;

	//part1添加普通和周期任务
	puts("Begin!");
	job1 = at_addjob(5,f1,"任务1打印",0);
	if(job1 < 0)
	{
		fprintf(stderr,"at_addjob():%s\n",strerror(-job1));
		exit(1);
	}
	job2 = at_addjob(2,f1,"周期打印",1);
	job3 = at_addjob(7,f1,"任务2打印",0);
	puts("End!");

	//part2暂停/恢复
	ret = at_stopjob(job2);
	if (ret < 0) {
		printf("暂停失败\n");
	}else {
		printf("开始暂停 3秒\n");
		for (i = 0; i < 3; i++) {
			write(1, "*", 1);
			sleep(1);
		}
		printf("\n");
	}
	at_resumejob(job2);
	printf("恢复暂停\n");


	//part3收尸 
	at_waitjob(job3);//
	at_waitjob(job1);//
	//周期性任务无法直接，要先cancel
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "job2 is cron can not waitjob\n");
	}
	//这样就可以回收了
	ret = at_canceljob(job2);
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "job2 is cron can not waitjob\n");
	}

	while(1)
	{
		write(1,".",1);
		sleep(1);
	}

	exit(0);
}


