#include <stdio.h>

int main(int argc, const char *argv[])
{
	char *a, *b;
	char arr[2]={1,2};

	a = &arr[0];
	b = &arr[1];
	printf("%p\n", a);
	printf("%p\n", b);
	return 0;
}
