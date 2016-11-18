#include <stdio.h>

int main(int argc, const char *argv[])
{
	extern int a, b;
	printf("a:%d,b:%d\n", a, b);
	return 0;
}

int a=100, b=200;
