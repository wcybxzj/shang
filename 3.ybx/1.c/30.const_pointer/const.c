#include <stdio.h>
#include <stdlib.h>

/* 只读变量(两者没区别)
 * const int a;
 * int const a;
 *
 * 常量指针(三者没区别)
 * p指向的内容不能改变
 * const int *p;
 * int const *p;
 * int *const p;
 *
 * 指针常量
 * const int *const p;
 */

//PI 只是简单替换,没有类型
#define PI 3.14

int main(int argc, const char *argv[])
{

#if 0	
	//part1:普通变量
	//const变量常量化, const让pi不能被修改
	const float pi = 3.14;
	pi = 123;//error
	//让一个指针p指向常量化pi变量的地址,就可以修改里面的内容
	float *p = &pi;/／为了达到目的少个开头的const
	*p = 3.1415;
	printf("%f\n",pi);
#endif

#if 0	
	//part2:
	int i = 1;
	int j = 100;
	const int *p = &i;
	//*p =123;//error
	p  = &j;
	printf("%d\n", *p);//100
#endif

#if 0
	//part3:
	int i = 1;
	int j = 100;
	int * const p = &i;
	*p = 123123;
	printf("%d\n", *p);
	//p = &j;//error
#endif

#if 0
	//part4:
	int i = 1;
	int j = 100;
	const int * const p = &i;//p和*p　都不能被修改
	printf("%d\n", *p);
#endif
	return 0;
}
