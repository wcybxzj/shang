#include <stdio.h>
int main(int argc, const char *argv[])
{
	//不知道数组类型求得数组个数
	int i;
	int arr[10]={};

	//打印数组地址 
	printf("arr %p\n", arr);
	printf("&arr[0] %p\n", &arr[0]);
	printf("&arr[1] %p\n", &arr[1]);
	printf("&(arr+1) %p\n", arr+1);
	printf("&arr[2] %p\n", &arr[2]);
	printf("&arr[3] %p\n", &arr[3]);
	printf("&arr[9] %p\n", &arr[9]);

	//方法1:
	printf("==========方法1==========\n");
	int total_len;
	int every_len;

	//arr 和 &arr 值一样但是含义不一样
	//arr  是指数组第一个数据的地址
	//&arr 是指整个数组的首地址
	printf("arr %p\n", arr);//两者的值相同
	printf("&arr %p\n", &arr);
	printf("&arr[1]%p\n", &arr[1]);
	printf("&arr+1 %p\n", &arr+1);
	printf("(&arr)+1 %p\n", (&arr)+1);//&arr

	//把指针类型进行统一下面才能计算
	int *begin= (void *)arr;
	int *end= (void *)(&arr+1);//两者的区别
	int *one= (void *)(arr+1);//

	//总长度
	printf("begin%p\n",begin);
	printf("end %p\n",end);
	total_len = end-begin;//40

	//每个长度
	//printf("%p\n", one);
	every_len = one-begin;//4
	printf("arr len is %d\n", total_len/every_len);

	printf("=============方法2==============\n");
	//方法2:
	printf("%lu\n",sizeof(arr)/sizeof(arr[0]));

	return 0;
}
