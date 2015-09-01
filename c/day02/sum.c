#include <stdio.h>

int main(void)
{
	int num;
	int i;
	int sum = 0;
	int res = 1;

	printf("input\n");
	scanf("%d", &num);

	for (i = num; i > 0; i--) {
		sum = sum + i;
		res = res * i;
	}	

	printf("sum = %d, res = %d\n", sum, res);

	return 0;
}
