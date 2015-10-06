#include <stdio.h>
#define M 3
int main(int argc, const char *argv[])
{
	int i;

	//1.数组初始化
	//int arr[M];//未初始化,自动变量里面虽机值
	//int arr[M]={};//全部初始化为0
	static int arr[M];//静态区数据自动初始化

	//2.arr就是数组的起始位置
	//arr就是一个地址常量,常量在程序执行过程中值不会发生变化的量
	//所以arr只能初始化不能无条件的出现在等号左边

	//arr = {1,2,3};//false


	printf("数组整体长度:%ld\n", sizeof(arr));
	printf("arr = %p\n",arr );

	for (i = 0; i < M; i++) {
		printf("&arr[i]:%p, arr[i]:%d\n", &arr[i], arr[i]);
	}

	printf("数组赋值\n");
	for (i = 0; i < M; i++) {
		scanf("%d",&arr[i] );
	}
	printf("after enter\n");
	for (i = 0; i < M; i++) {
		printf("&arr[i]:%p, arr[i]:%d\n", &arr[i], arr[i]);
	}
	return 0;
}
