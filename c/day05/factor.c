#include <stdio.h>

long factor(long num)
{
	if (num == 0) {
		return 1;
	}
	return num * factor(num-1);
}

int main(void)
{
	printf("*****%ld*******\n", factor(6));	

	return 0;
}
