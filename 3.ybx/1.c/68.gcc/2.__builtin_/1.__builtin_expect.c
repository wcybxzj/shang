#include <stdio.h>
#include <stdlib.h>
//likely unlikely都是gcc内部宏
//预测x是1的几率大
#define  likely(x)        __builtin_expect(!!(x), 1) 
//预测x是0的几率小
#define  unlikely(x)      __builtin_expect(!!(x), 0)

int main(int argc, const char *argv[])
{
	if (argc!=2) {
		printf("./a.out 123\n");
		exit(1);
	}

	int x, y; 
	x = atoi(argv[1]);
	//1.无论unlikely还是likely 都是表示一个你对的预测
	//实际仍是if(x>0){}
	//2.现在y会预先加载成-1,来对代码进行优化
	if(unlikely(x > 0)) 
		y = 1; 
	else 
		y = -1;

	printf("y:%d\n", y);
	return 0;
}

