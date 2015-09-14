#include <stdio.h>

int main(int argc, const char *argv[])
{
	int a = 123;
	printf("%#x\n", &a);
	printf("%p\n", &a);
	return 0;
}
