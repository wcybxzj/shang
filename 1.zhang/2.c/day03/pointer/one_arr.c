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

	printf("==========方法1==========\n");
	printf("==========方法1==========\n");
	printf("==========方法1==========\n");
	int total_len;
	int every_len;

	//arr 和 &arr 值一样但是含义不一样
	//arr  是指数组第一个数据的地址 &arr[0]
	//&arr 是指整个数组的首地址
	printf("arr %p\n", arr);		//0x7fff54751820
	printf("&arr[0] %p\n", &arr[0]);//0x7fff54751820
	printf("&arr %p\n", &arr);		//0x7fff54751820

	printf("arr+1 %p\n", arr+1);		//0x7fff54751824
	printf("&arr[0]+1 %p\n", &arr[0]+1);//0x7fff54751824

	printf("&arr+1 %p\n", &arr+1);	  //0x7fff54751848
	printf("(&arr)+1 %p\n", (&arr)+1);//0x7fff54751848

	//把指针类型进行统一下面才能计算
	int begin = arr;
	int end = (&arr+1);
	int one = (arr+1);

	printf("指针类型如果加1 地址的增加是按照指针类型进行的\n");
	printf("一旦类型转为int +1 操作就会变成普通的加1:\n");
	printf("begin +1:%d\n", begin+1); //1486755873

	printf("begin	:%d\n",begin);//1486755872
	printf("end	 	:%d\n",end);//1486755912
	printf("one  	:%d\n", one);//1486755876

	//总长度
	total_len = end-begin;//40
	printf("total_len is %d\n", total_len);

	//每个长度
	every_len = one-begin;//4
	printf("every_len is %d\n", every_len);
	printf("arr len is %d\n", total_len/every_len);

	printf("=============方法2==============\n");
	printf("=============方法2==============\n");
	printf("=============方法2==============\n");
	printf("sizeof(arr) %lu\n", sizeof(arr));
	printf("sizeof(arr[0]) %lu\n", sizeof(arr[0]));
	printf("%lu\n",sizeof(arr)/sizeof(arr[0]));

	return 0;
}
