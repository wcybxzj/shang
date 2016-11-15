#include <stdio.h>
#include <stdlib.h>

/* 只读变量(两者没区别)
 * const int a; //a是只读
 * int const a; //a是只读
 *
 * 常量指针
 * const int *p; *p read-only
 * int const *p; *p read-only
 * int *const p;  p read-only(!!!!!!!!!!!!)
 *
 * 指针常量
 * const int *const p;
 */

//PI 只是简单替换,没有类型
#define PI 3.14

int main(int argc, const char *argv[])
{

printf("===============test 1:const int p=========================\n");
#if 0
	const int pi = 3;
	//pi = 123;//error  read-only variable pi
	int *p = &pi;//为了达到目的少个开头的const
	*p = 5;
	printf("%d\n",pi);
#endif

printf("===============test 2:int const  p=========================\n");
#if 0
	int const pi = 3;
	pi = 123;//error read-only variable pi
	//int *p = &pi;//为了达到目的少个开头的const
	//*p = 5;
	printf("%d\n",pi);
#endif

printf("===============test 3:const int *p=========================\n");
#if 0	
	//part2:
	int i = 1;
	int j = 100;
	const int *p = &i;
	printf("sizeof:%d\n", sizeof(const int *));//8
	//*p =123;//error ,*p是read-only
	p  = &j;
	printf("%d\n", *p);//100
#endif

printf("===============test 4:int const *p====================\n");
#if 0	
	int i = 1;
	int j = 100;
	int const *p = &i;
	printf("sizeof:%d\n", sizeof(int const *));//8
	//*p =123;//error ,*p是read-only
	p  = &j;
	printf("%d\n", *p);//100
#endif
 
printf("===============test 5:int *const p=========================\n");
#if 0
	int i = 1;
	int j = 100;
	int * const p = &i;
	printf("sizeof:%d\n", sizeof(int *const ));//8
	//*p = 123123;
	p = &j;//error ,p是 read-only
	printf("%d\n", *p);
#endif

printf("===============test 6:int *const p=========================\n");
#if 0
	int i = 1;
	int j = 100;
	const int * const p = &i;//p和*p　都不能被修改
	//*p =123;//error ,*p read-only
	//p = &j;//error, p read-only
	printf("%d\n", *p);
#endif
	return 0;
}
