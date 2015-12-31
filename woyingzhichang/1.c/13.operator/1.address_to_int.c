#include <stdio.h>
int main(int argc, const char *argv[])
{
	int a=123;
	int *p = &a;
	int c = (int) p;
	printf("%p\n", &a);
	printf("%p\n", p);
	printf("%x\n",c);
	return 0;
}
