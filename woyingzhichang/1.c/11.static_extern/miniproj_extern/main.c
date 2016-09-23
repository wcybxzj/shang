#include <stdio.h>
#include <stdlib.h>
#include "proj.h"

int i = 10;
int main(int argc, const char *argv[])
{
	//test1：func()的修改不能影响定义的文件中的i
	printf("[%s] i:%d\n", __FUNCTION__, i);
	func();
	printf("[%s] i:%d\n", __FUNCTION__, i);

	//test2:定义中的i 能影响引用文件中的i
	i =123;
	func2();
	exit(0);
}
