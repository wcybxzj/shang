#include <stdio.h>
#if 0
	1.未初始化的全局变量和局部静态变量，在内存bss段
	2.初始化的全局变量和局部静态变量,在内存的data段
#endif

int glob;
int glob_2;//0
int main(void)
{
	int var;
	int c = 6;
	static int d;//0
	int glob = 10;
	
	c = d + c;

	//glob = 10, glob_2 = 0, var = 1684111414, c = 6, d = 0
	printf("glob = %d, glob_2 = %d, var = %d, c = %d, d = %d\n", glob, glob_2 ,var, c, d);

	return 0;
}
