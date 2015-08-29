#include <stdio.h>
#include <stdlib.h>

/*
 *	缓冲区：大多数情况下，缓冲区的存在是好事，作用是合并系统调用
 *
 *	行缓冲：换行的时候刷新，满了的时候刷新，强制刷新（标准输出是这样的，因为涉及到终端设备）
 *
 *	全缓冲：满了的时候刷新，强制刷新（默认，只要不是终端设备）
 *
 *	无缓冲：stderr
 * */

int main()
{
	int i;

	i = 1;

	printf("[%s:%d]before while()",__FUNCTION__,__LINE__);
	fflush(stdout);
//  fflush(NULL);

	while(1);

	printf("[%s:%d]after while()",__FUNCTION__,__LINE__);
	fflush(stdout);

	printf("i = %d",i);

	exit(0);
}


