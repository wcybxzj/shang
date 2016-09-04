#include <stdio.h>
#include <stdlib.h>

/*
					char	short	int	 long	float	double	long long	long double
Linux-64	长度	1		2		4	 8		4		8		8			16
*/

//http://blog.csdn.net/liukun321/article/details/6974282
//http://www.cnblogs.com/luxiaoxun/archive/2012/11/09/2762438.html

//原则A：struct或者union的成员，第一个成员在偏移0的位置，之后的每个成员的起始位置必须是当前成员大小的整数倍；
//原则B：如果结构体A含有为结构体类型的成员B，那么B的起始位置必须是B中最大元素大小整数倍地址；
//原则C：结构体的总大小，必须是内部最大成员的整数倍；

struct my_struct1{
	char a;//1+15
	long double b;//16
}test1;//32

#pragma pack(2)
struct my_struct2{
	char a;//1+1
	long double b;//16
}test2;//18
#pragma pack()

struct my_struct3{
	char a;//1+7
	double b;//8 8
	char c;//1
}test3;//24

struct my_struct{
	char a[11];//11+1
	int b;//4
	char c;//1
}test4;//20

struct my_test{
	int a;
	char b;
}test5;//8

struct my_struct6{
	struct my_test a;//8
	double b;//8
	int c;//4
	char d;//1
}test6;//24

struct my_struct7{
	struct my_test a;//8+8
	long double b;//16
	int c;//4
	char d;//1+7
}test7;//44+4

struct my_struct8{
	char a;//1+3
	struct my_test b;//8
}test8;//

struct my_test9{
	long double a;
	char b;
}test9;//32

struct my_test10{
	int a;
	char b;
	struct my_test9 c;
}test10;//48

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

	printf("test1:%d\n", sizeof(test1));
	printf("test2:%d\n", sizeof(test2));
	printf("test3:%d\n", sizeof(test3));
	printf("test4:%d\n", sizeof(test4));
	printf("test5:%d\n", sizeof(test5));
	printf("test6:%d\n", sizeof(test6));
	printf("test7:%d\n", sizeof(test7));
	printf("test8:%d\n", sizeof(test8));
	printf("test9:%d\n", sizeof(test9));
	printf("test10:%d\n", sizeof(test10));
}
