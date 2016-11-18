#include <stdio.h>
#include "proj.h"

int i;
void func()
{
	i =222;
	printf("[%s] i:%d\n", __FUNCTION__, i);
}

void func2()
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
}
