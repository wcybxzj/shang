#include <stdio.h>
#include <stdlib.h>

/*
					char	short	int	 long	float	double	long long	long double
Linux-64	长度	1		2		4	 8		4		8		8			16
*/


/*
原则A:
struct或者union的成员，第一个成员在偏移0的位置，之后的每个成员的起始位置必须是当前成员大小的整数倍,
也就是起始地址 % sizeof(type) == 0 

原则B:
如果一个结构里有某些结构体成员，则结构体成员要从其内部最大元素大小的整数倍地址开始存储。
(struct a里存有struct b，b里有char，int，double等元素，那b应该从8的整数倍开始存储)。

原则C:
结构体的总大小，必须是内部最大成员的整数倍；
(结构体A内嵌套结构体B,结构体B不能算最大成员,结构体B中基本类型才机会算)
*/

//1.char在任何位置都能存

//2.结构体从0开始为存储地址
//判断一个类型能够存储的条件address%sizeof(type)=0

//3.结构体的总大小要能整除,结构体中最大类型的尺寸

//4.结构体A中嵌套机构体B
//结构体B按照 B中最大数据类型来进行内存对齐

//OFFSET中 &和-> 是右结合性,->先运行
//  ((char *)&( ((struct *)0)->member ) - (char *)0)
#define PRINT_D(intValue)     printf(#intValue" is %d\n", (intValue))
#define OFFSET(struct,member)  ((char *)&((struct *)0)->member - (char *)0)

void test1()
{
	typedef struct _my_struct1 {
		char a;//1+15
		long double b;//16
	} my_struct1;//32

	//本来应该是a:1+15
	//b才能存入进来
	//
	//这里强制以2来进行对齐
	//a:1+1达到2, b就存进来了
	#pragma pack(2)
	typedef struct{
		char a;//1+1
		long double b;//16
	}   my_struct2;//18
	#pragma pack()

	typedef struct{
		char a;//1+7
		double b;//8
		char c;//1
	} my_struct3;//24

	typedef struct  {
		double a;//8
		char b;//1+7
		double c;//8 8
	} my_struct4;//24

	typedef struct {
		char a;//1+3
		int b;//4
		char c;//1+3
	} my_struct5;//12

	typedef struct {
		int a;//4
		char b;//3+1
		int c;//4
	} my_struct6;//12

	typedef struct {
		char a[11];//11+1
		int b;//4
		char c;//1
	}  my_struct7;//20
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct1));
	PRINT_D(OFFSET(my_struct1, a));
	PRINT_D(OFFSET(my_struct1, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct2));
	PRINT_D(OFFSET(my_struct2, a));
	PRINT_D(OFFSET(my_struct2, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct3));
	PRINT_D(OFFSET(my_struct3, a));
	PRINT_D(OFFSET(my_struct3, b));
	PRINT_D(OFFSET(my_struct3, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct4));
	PRINT_D(OFFSET(my_struct4, a));
	PRINT_D(OFFSET(my_struct4, b));
	PRINT_D(OFFSET(my_struct4, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct5));
	PRINT_D(OFFSET(my_struct5, a));
	PRINT_D(OFFSET(my_struct5, b));
	PRINT_D(OFFSET(my_struct5, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct6));
	PRINT_D(OFFSET(my_struct6, a));
	PRINT_D(OFFSET(my_struct6, b));
	PRINT_D(OFFSET(my_struct6, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct7));
	PRINT_D(OFFSET(my_struct7, a));
	PRINT_D(OFFSET(my_struct7, b));
	PRINT_D(OFFSET(my_struct7, c));
}

void test2()
{
	//1+3+4=8
	typedef struct  {
		char b;//1+3
		int a;//4
	} my_struct8;//8

	//存储b的情况:
	//my_struct8 b存储时候要按照里面最大类型来决定
	//my_struct8 最大的是int, 所以my_struct8 b要按照4来进行内存对齐
	//存储c的情况:
	//a+b=12 12%sizeof(double)=4, c存不进去
	//a+b+4=16 16%8=0才行
	//16+8=24
	typedef struct{
		char a;//1+3
		my_struct8 b;//8+4
		double c;//8
	} my_struct9;//24

	typedef struct  {
		char a;//1+3
		my_struct8 b;//8
		my_struct8 c;//8
	} my_struct10;//20

	//8+8+4+1=21 21%sizeof(double)=5
	//21+3=24    24%sizeof(double)=0
	typedef struct {
		my_struct8 a;//8
		double b;//8
		int c;//4
		char d;//1
	}  my_struct11;//24

	//8+1=9 ,9%sizeof(int)=1
	//9+3=12 12%sizeof(int)=0
	typedef struct{
		my_struct8 a;//8
		char b;//1
	} my_struct12;//12

	typedef struct  {
		char a;//1+3
		my_struct8 b;//8
	}  my_struct13;//12

	typedef struct{
		int a;//4
		char b;//1
		char c;//1
	} my_struct14;//8

	//37%16不等于0
	//48%16等于0
	typedef struct {
		my_struct8 a;//8+8
		long double b;//16
		int c;//4
		char d;//1
	} my_struct15;//48

	typedef struct {
		long double a;//16
		char b;//1
	} my_struct16;//32

	typedef struct {
		int a;//4
		char b;//1+11
		my_struct16 c;//32
	} my_struct17;//48

	//my_struct16其中最大类型是16字节,
	//这个类型存储就以16来内部才能对齐
	typedef struct {
		int a;//4+12
		my_struct16 b;//32
		char c;//1
	} my_struct18;//64

	typedef struct {
		char a;
		char b;
	} my_struct19;//2

	PRINT_D(sizeof(my_struct8));
	PRINT_D(OFFSET(my_struct8, a));
	PRINT_D(OFFSET(my_struct8, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct9));
	PRINT_D(OFFSET(my_struct9, a));
	PRINT_D(OFFSET(my_struct9, b));
	PRINT_D(OFFSET(my_struct9, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct10));
	PRINT_D(OFFSET(my_struct10, a));
	PRINT_D(OFFSET(my_struct10, b));
	PRINT_D(OFFSET(my_struct10, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct11));
	PRINT_D(OFFSET(my_struct11, a));
	PRINT_D(OFFSET(my_struct11, b));
	PRINT_D(OFFSET(my_struct11, c));
	PRINT_D(OFFSET(my_struct11, d));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct12));
	PRINT_D(OFFSET(my_struct12, a));
	PRINT_D(OFFSET(my_struct12, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct13));
	PRINT_D(OFFSET(my_struct13, a));
	PRINT_D(OFFSET(my_struct13, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct14));
	PRINT_D(OFFSET(my_struct14, a));
	PRINT_D(OFFSET(my_struct14, b));
	PRINT_D(OFFSET(my_struct14, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct15));
	PRINT_D(OFFSET(my_struct15, a));
	PRINT_D(OFFSET(my_struct15, b));
	PRINT_D(OFFSET(my_struct15, c));
	PRINT_D(OFFSET(my_struct15, d));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct16));
	PRINT_D(OFFSET(my_struct16, a));
	PRINT_D(OFFSET(my_struct16, b));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct17));
	PRINT_D(OFFSET(my_struct17, a));
	PRINT_D(OFFSET(my_struct17, b));
	PRINT_D(OFFSET(my_struct17, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct18));
	PRINT_D(OFFSET(my_struct18, a));
	PRINT_D(OFFSET(my_struct18, b));
	PRINT_D(OFFSET(my_struct18, c));
	printf("===============================================\n");
	PRINT_D(sizeof(my_struct19));
	PRINT_D(OFFSET(my_struct19, a));
	PRINT_D(OFFSET(my_struct19, b));
}



//64位linux
//char 	      :1
//short	   	  :2
//int     	  :4
//long	      :8
//float	   	  :4
//double  	  :8
//long long	  :8
//long double :16
int main(int argc, const char *argv[])
{
	printf("char 	    :%d\n",	sizeof(char));
	printf("short	   	:%d\n",	sizeof(short));
	printf("int	     	:%d\n",	sizeof(int));
	printf("long	    :%d\n",	sizeof(long));	
	printf("float	   	:%d\n",	sizeof(float));
	printf("double	  	:%d\n",	sizeof(double));
	printf("long long	:%d\n",	sizeof(long long));
	printf("long double :%d\n",	sizeof(long double));
	printf("===============================================\n");
	//test1();
	test2();
	printf("===============================================\n");
}
