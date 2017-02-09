#include <stdio.h>
//http://www.cnblogs.com/luxiaoxun/archive/2012/11/09/2762438.html

#define PRINT_D(intValue)     printf(#intValue" is %d\n", (intValue))
#define OFFSET(struct,member)  ((char *)&((struct *)0)->member - (char *)0)

//按1字节对齐，结构体成员紧密排列
//
//sizeof(student) is 7
//OFFSET(student,sex) is 0
//OFFSET(student,score) is 1
//OFFSET(student,age) is 3
void test1()
{
	#pragma pack(1)
	typedef struct
	{
		char    sex;
		short   score;
		int     age;
	}student;
	#pragma pack()
	PRINT_D(sizeof(student));
    PRINT_D(OFFSET(student,sex));
    PRINT_D(OFFSET(student,score));
    PRINT_D(OFFSET(student,age));
}

//#pragma pack(4)语句后,实际对齐原则是
//自身对齐值(成员sizeof大小)和指定对齐值(#pragma pack指定的对齐大小)的较小者。
//score成员自身对齐值为2，指定对齐值为4，实际对齐为2；
//age自身对齐值为4，指定对齐为4，所以实际对齐值为4；
//前面的sex和score正好占用4字节，所以age接着存放
//
//sizeof(student) is 8
//OFFSET(student,sex) is 0
//OFFSET(student,score) is 2
//OFFSET(student,age) is 4
void test2()
{
	#pragma pack(4)
	typedef struct
	{
	    char    sex;//1+1
	    short   score;//2
	    int     age;//4
	}student;
	#pragma pack()
	PRINT_D(sizeof(student));
    PRINT_D(OFFSET(student,sex));
    PRINT_D(OFFSET(student,score));
    PRINT_D(OFFSET(student,age));
}


int main(int argc, const char *argv[])
{
	//test1();
	test2();
	return 0;
}
