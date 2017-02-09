#include <stdio.h>

//http://blog.csdn.net/liukun321/article/details/6974282

 //原则一：结构体中元素是按照定义顺序一个一个放到内存中去的，
 //但并不是紧密排列的。
 //从结构体存储的首地址开始，每一个元素放置到内存中时，
 //它都会认为内存是以它自己的大小来划分的，
 //因此元素放置的位置一定会在自己宽度的整数倍上开始
 //（以结构体变量首地址为0计算）。

// 原则二：在经过第一原则分析后，
// 检查计算出的存储单元是否为所有元素中最宽的元素的长度的整数倍，
// 是，则结束；若不是，则补齐为它的整数倍。


//OFFSET中 &和-> 是右结合性,->先运行
//((char *)&( ((struct *)0)->member ) - (char *)0)
#define PRINT_D(intValue)     printf(#intValue" is %d\n", (intValue))
#define OFFSET(struct,member)  ((char *)&((struct *)0)->member - (char *)0)

void test8()
{
	//(1+3)+4+8=16
	typedef struct  {
		char a1;
		int b1;
		double c1;
	} X;

	//在存放第二个元素b时的初始位置是在double型的长度8的整数倍处，
	//而非16的整数倍处，即系统为b所分配的存储空间是第8~23个字节。
	//(1+7)+16=24
	typedef struct  {
		char a;//1
		X b;//16
	} Y;
	PRINT_D(sizeof(Y));//24
	PRINT_D(OFFSET(Y, a));//0
	PRINT_D(OFFSET(Y, b));//8

	//16+1=17, 最大原始double 8
	//结构体整体长度要能被8整除
	typedef struct  {
		X a;
		char b;
	}Z;

	PRINT_D(sizeof(Z));//24
	PRINT_D(OFFSET(Z, a));//0
	PRINT_D(OFFSET(Z, b));//16
}

void test9()
{
	//(1+7)+8+4=20
	//20不能被8整除
	//24
	typedef struct  {
		char a1;
		double b1;
		int c1;
	} X;//24
	PRINT_D(sizeof(X));//24
	PRINT_D(OFFSET(X, a1));//0
	PRINT_D(OFFSET(X, b1));//8
	PRINT_D(OFFSET(X, c1));//16

	//(1+7)+20=28
	//28不能被8整除
	typedef struct  {
		char a;//1
		X b;//20
	} Y;
	PRINT_D(sizeof(Y));//32
	PRINT_D(OFFSET(Y, a));//0
	PRINT_D(OFFSET(Y, b));//8

	//25-->32
	typedef struct  {
		X a;//24
		char b;//1
	} Z;
	PRINT_D(sizeof(Z));//32
	PRINT_D(OFFSET(Z, a));//0
	PRINT_D(OFFSET(Z, b));//24
}

int main(int argc, const char *argv[])
{
	//test8();
	test9();
	return 0;
}
