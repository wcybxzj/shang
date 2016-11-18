#include <stdio.h>
#include <stdlib.h>
#include "proj.h"

//main.c定义全部变量i
//proj.h 告送proj.c 全局变量i是在其他中定义的
//main.c和proj.c 共享变量i都可以修改它的值

/*
./main 
[main] i:10
[func] i:222
[main] i:222
[func2] i:123
*/
int i = 10;
int main(int argc, const char *argv[])
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
	func();
	printf("[%s] i:%d\n", __FUNCTION__, i);

	i =123;
	func2();
	exit(0);
}
