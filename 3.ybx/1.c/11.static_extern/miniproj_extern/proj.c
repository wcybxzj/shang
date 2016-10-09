#include <stdio.h>
#include "proj.h"

void func()
{
	i=234;
	printf("[%s] i:%d\n", __FUNCTION__, i);
}

void func2()
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
}
