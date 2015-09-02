#include <stdio.h>
#if 0
	函数:
		1.函数的定义:功能
		2.函数的声明:函数的返回值类型，函数的参数类型
		3.函数的调用
	注意的：
		1.函数的返回值如果不写，表示的是int
		2.函数的传参是值传递的过程
		3.类型匹配
		4.函数的参数就是局部变量
#endif

int jiec(int );
int jiec_r(int num, int *res);

int main(void)
{
	int ji;

	ji = jiec(5);
	printf("******%d*******\n", ji);

	if (jiec_r(-6, &ji) < 0) {
		printf("input error\n");
	} else {
		printf("******%d*******\n", ji);
	}

	return 0;
}

//函数的定义
int jiec(int num)
{
	int ret = 1;

	for ( ; num > 1; num --) {
		ret *= num;
	}

	return ret;	
}

int jiec_r(int num, int *res)
{
	int tmp = 1;

	if (num <= 0) {
		return -1;
	}

	for ( ; num > 1; num --) {
		tmp *= num;
	}
	*res = tmp;

	return 0;
}

