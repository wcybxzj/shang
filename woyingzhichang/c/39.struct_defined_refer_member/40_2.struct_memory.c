#include <stdio.h>
/*
					char	short	int	 long	float	double	long long	long double
Win-32	    长度	1		2		4	 4		4		8		8			8
		    模数	1		2		4	 4		4		8		8			8

Linux-32	长度	1		2		4	 4		4		8		8			12
			模数	1		2		4	 4		4		4		4			4

Linux-64	长度	1		2		4	 8		4		8		8			16
			模数	1		2		4	 8		4		8		8			16
*/


//example1:
struct my_struct1{
	char a;//1+3  1+15
	long double b;//12 16
}test1;// 32bit:16 64bit:32

//example2:
#pragma pack(2)
struct my_struct2{
	char a;//1+1
	long double b;//12 16
}test2;//14 18
#pragma pack()

//example3:
struct my_struct3{
	char a;//1+3 1+7
	double b;//8 8
	char c;//1
}test3;//16 24

//struct my_struct{
//	char a;//1+3
//	double b;
//	char c;
//} test4;
//
//
//struct my_test{
//	int a;
//	char b;
//};//8
//
//struct my_struct{
//	struct my_test a;//8
//	double b;//8
//	int c;//4
//	char d;//1
//}st5_var;//24

int main(int argc, const char *argv[])
{
	printf("%d\n", sizeof(test1));
	printf("%d\n", sizeof(test2));
	printf("%d\n", sizeof(test3));
}
