#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void int_handler(int s){
	write(1, "!",1);
}

//一直按Ctrl+C,整个本来计划执行20秒的程序1秒就执行完
//程序要想终止 只能Ctrl+\ 终止程序,CTRL+C的默认动作被修改了

//观察1:write没有被中断而是sleep被中断如果补救
//观察write的返回值没问题1,sleep返回值是大于0

//观察2:一直ctrl+c程序一直在sleep, sleep被信号中断后,

int main(int argc, const char *argv[])
{
	int i, ret, num = 1;
	//signal(SIGINT, SIG_IGN);//忽略此信号
	signal(SIGINT, int_handler);

	for (i = 0; i < 10; i++) {
		ret = write(1, "*", 1);
		//printf("write  返回值来确定是否被中断%d\n", ret);

		while ( (ret = sleep(num)) > 0 ) {
			printf("i is %d,sleep was interrupted elapsed %d\n",i , ret);
			sleep(ret);
		}
	}
	return 0;
}
