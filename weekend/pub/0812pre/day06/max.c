#include <stdio.h>

int max_y(int a, int b);

int main(void)
{
	int m, n;

	printf("input two numbers:");
	scanf("%d%d", &m, &n);

	printf("%d and %d 的最大公约数%d\n", m, n, max_y(m, n));	

	return 0;
}

int max_y(int a, int b)
{
	int ret;

	while ((ret = a % b) != 0) {
		a = b;
		b = ret;	
	}
	return b;
}

