#include <stdio.h>

int sum(int , int );
int main(void)
{
	int var1, var2;
	int ret;

	printf("input two numbers:");
	scanf("%d%d", &var1, &var2);

	//1 2
	ret = sum(var1, var2);

	printf("%d + %d = %d\n", var1, var2, ret);

	return 0;
}

int sum(int a, int b)
{
	return a+b;
}

