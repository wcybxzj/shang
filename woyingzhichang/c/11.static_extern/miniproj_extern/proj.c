#include <stdio.h>

extern int i;
void func()
{
	i=234;
	printf("[%s] i:%d\n", __FUNCTION__, i);
}
