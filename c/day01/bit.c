#include <stdio.h>

int main(void)
{
	int var = 132;
	//10000100

	printf("%d\n", (1u<<3));
	printf("%d\n", ~(1u<<2));

	var = var | (1u<<3);
	//10001100
	printf("******%d*******\n", var);
	var = var & (~(1u<<3));
	printf("******%d*******\n", var);

	return 0;
}
