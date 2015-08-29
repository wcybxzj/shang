#include <stdio.h>
#if 0
	1.未初始化的全局变量和局部静态变量，在内存bss段
	2.初始化的全局变量和局部静态变量,在内存的data段
#endif

int glob;

int main(void)
{
	int var;
	int c = 6;
	static int d;
	int glob = 10;
	
	c = d + c;

	printf("glob = %d, var = %d, c = %d, d = %d\n", glob, var, c, d);

	return 0;
}
