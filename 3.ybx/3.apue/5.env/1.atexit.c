#include <stdio.h>
#include <stdlib.h>

static void f1(void)
{
	puts("f1 is working!");
}

static void f2(void)
{
	puts("f2 is working!");
}

static void f3(void)
{
	puts("f3 is working!");
}

int main(int argc, const char *argv[])
{

	puts("begin");


	atexit(f1);
	atexit(f2);
	atexit(f3);

	return 0;
}
