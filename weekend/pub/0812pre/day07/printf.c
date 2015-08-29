#include <stdio.h>

int main(void)
{
	int a = 5;
	int b = 3;

	printf("%d\n", b, b++, a=a+b);
	printf("a = %d, b = %d\n", a, b);

	return 0;
}
