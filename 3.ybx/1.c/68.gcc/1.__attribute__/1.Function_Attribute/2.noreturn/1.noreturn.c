#include <stdio.h>
#include <stdlib.h>
extern void myexit(void) __attribute__((__noreturn__));

void myexit(void)
{
	exit(0);
}

int test(int n)
{
	if ( n > 0 )
	{
		myexit();
		/* 程序不可能到达这里*/
	}
	else
		return 0;
}

//测试1:
//gcc 1.noreturn.c -Wall
//1.noreturn.c: In function ‘test’:
//1.noreturn.c:17: warning: control reaches end of non-void function

//解决办法:__attribute__((noreturn))
int main(int argc, const char *argv[])
{
	test(1);
	test(0);
	return 0;
}
