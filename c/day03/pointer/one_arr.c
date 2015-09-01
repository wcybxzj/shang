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

	printf("arr %p\n", arr);
	printf("&arr %p\n", &arr);
	printf("&arr[1]%p\n", &arr[1]);
	printf("&arr+1 %p\n", &arr+1);
	printf("(&arr)+1 %p\n", (&arr)+1);//&arr

	char *begin= (void *)arr;
	char *end= (void *)(&arr+1);
	char *one= (void *)(arr+1);

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
	printf("%d\n",sizeof(arr)/sizeof(arr[0]));

	return 0;
}
