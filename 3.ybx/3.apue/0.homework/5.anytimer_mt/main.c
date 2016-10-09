#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "anytimer.h"
void* f1(void *p)
{
	//sleep(10);
	puts(p);
	fflush(NULL);
}


void* f2(void *p)
{
        int i;
        for(i=0;i<10;i++){
                write(1, "^", 1);
                sleep(1);
        }
        puts(p);
}

void v1_test();
void v4_test();

//如果子任务执行时间过长 整个程序执行就会卡在那不动，子进程比较麻烦,觉得用脱离状态的线程
int main()
{ 	
	v4_test();
	exit(0);
}

/*
Begin!
End!
id 1 开始暂停 3秒
*id 0 打印1秒
*$*$
id 1 恢复暂停
waitjob id 2 判断状态改变
waitjob id 2 等待状态改变
$$id 1 打印2秒
$$id 1 打印2秒
waitjob id 2  释放资源
waitjob id 0 判断状态改变
waitjob id 0  释放资源
id 1 is 周期任务 can not waitjob
waitjob id 1 判断状态改变
waitjob id 1  释放资源
.id 2 打印7秒
.$.$.$.$.$.$.$.$^C
*/
void v1_test(){
	//Begin!End!..bbb...aaa..ccc........
	int job1,job2,job3;
	int ret, i;

	//part1添加普通和周期任务
	puts("Begin!");
	job1 = at_addjob(1,f1,"id 0 打印1秒",NOREPEAT);
	if(job1 < 0)
	{
		fprintf(stderr,"at_addjob():%s\n",strerror(-job1));
		exit(1);
	}
	job2 = at_addjob(2,f1,"id 1 打印2秒",REPEAT);
	job3 = at_addjob(7,f1,"id 2 打印7秒",NOREPEAT);
	puts("End!");

	//part2暂停/恢复
	ret = at_stopjob(job2);
	if (ret < 0) {
		printf("id 1 暂停失败\n");
	}else {
		printf("id 1 开始暂停 3秒\n");
		for (i = 0; i < 3; i++) {
			write(1, "*", 1);
			sleep(1);
		}
		printf("\n");
	}
	at_resumejob(job2);
	printf("id 1 恢复暂停\n");


	//part3收尸 
	at_waitjob(job3);//
	at_waitjob(job1);//
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "id 1 is 周期任务 can not waitjob\n");
	}
	//这样就可以回收了
	ret = at_canceljob(job2);
	ret = at_waitjob(job2);
	if (ret < 0) {
		fprintf(stderr, "id 1 is 周期任务 can not waitjob\n");
	}

	while(1)
	{
		write(1,".",1);
		sleep(1);
	}
}

//主要是说明信号版本的anytimer在遇到执行函数执行时间长时候的问题
//在多线程版本让执行函数单独使用一个线程避免1个job函数执行时间影响job_arr中其他线程的计数
void v4_test()
{
        int job1,job2,job3;
        int ret, i;

        puts("Begin!");
        job1 = at_addjob(5, f2, "aaa", NOREPEAT);
        job2 = at_addjob(2, f1, "bbb", NOREPEAT);
        job3 = at_addjob(7, f1, "ccc", NOREPEAT);
        puts("End!");

        while (1) {
                write(1, ".", 1);
                sleep(1);
        }

}
