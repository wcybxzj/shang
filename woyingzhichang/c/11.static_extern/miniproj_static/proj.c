#include <stdio.h>

static int i = 100;

static void func()
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
}

void call_func()
{
	func();
}
