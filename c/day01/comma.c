#include <stdio.h>

int main(void)
{
	int a = 10;
	int b = 5;
	int c;

	c = (a+=1, b+a, 18);

	printf("a = %d, b = %d, c = %d\n", a, b, c);

	return 0;
}
