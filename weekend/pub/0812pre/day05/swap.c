#include <stdio.h>

void swap(int *a, int *b);

int main(void)
{
	int var1 = 10;
	int var2 = 20;

	printf("before change: var1 = %d, var2 = %d\n", var1, var2);

	swap(&var1, &var2);

	printf("after change: var1 = %d, var2 = %d\n", var1, var2);

	return 0;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

