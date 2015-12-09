#include <stdio.h>

enum day{
	MON,//0
	TUS,//1
	WES = 0,
	THR,//1
	FRI,//2
	SAT,//3
	SUN,//4
}var2 = SUN;

/*
//error 前面有MON .....
enum day1{
	MON = 1,//1
	TUS,
	WES,
	THR=1,
	FRI,
	SAT,
	SUN,//7
};
*/

//覆盖前面的TUS
//#define TUS 222

//习惯当成一组连续的常量用
//好处1:不用手动初始化数字
//好处2:gcc -E 下面的值不会替换成数字方便调试
enum{
	STATE_RUNNING =1,
	STATE_CANCEL,
	STATE_OVER
};

struct job_st{
	int id;
	int state;
};

void normal_use()
{
	printf("%d\n", MON);//0
	printf("%d\n",WES);//0

	printf("%d\n", TUS);//1

	enum day var1 = FRI;
	printf("%d\n", var1);//2

	printf("%d\n", var2);//4
}


void habit_use()
{
	struct job_st job1;

	//获取任务状态

	switch(job1.state){
		case STATE_RUNNING:
		break;

		case STATE_CANCEL:
		break;

		case STATE_OVER:
		break;

		default:
			//abort();
		break;

	}
}

int main(int argc, const char *argv[])
{
	normal_use();
	return 0;
}
