#include <stdio.h>
#include "func.h"
//gcc main.c func.c
//这个例子的意思是想说main引用 .h是需要声明
//func.c没引用func.h是因为不需要引用

//也可以先生成.o文件,和上面效果一样
//gcc -c main.c
//gcc -c func.c
//gcc main.o func.o

int main(int argc, const char *argv[])
{
	func();
	return 0;
}
