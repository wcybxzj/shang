#include <stdio.h>
#include <stdlib.h>
/*
 *	缓冲区的作用：绝大多数情况下，缓冲区的存在是件好事，作用是合并系统调用
 *	行缓冲：换行的时候刷新，满了的时候刷新，强制刷新（标准输出就是这样的，因为涉及到了终端设备）。
 *
 *	全缓冲：满了的时候刷新，强制刷新。（默认，只要不是终端设备就全部采用全缓冲模式）
 *
 *	无缓冲：stderr，需要立即输出
 *
 *
 * */

int main()
{
	int i;
	
	i = 1;

	printf("[%s:%d]:Before while()",__FUNCTION__,__LINE__);	
	fflush(stdout);

	while(1);

	printf("[%s:%d]:After while()",__FUNCTION__,__LINE__);
	fflush(stdout);

	exit(0);
}



