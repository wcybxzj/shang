#include <stdio.h>

int main(void)
{
	int var, tmp;
	int ret = 1;
	int sum = 0;

	printf("input: ");
	scanf("%d", &var);

	tmp = var;
	for ( ;var >= 1; var--) {
		ret = ret * var;
		sum +=  var;
	}

	printf("%d 的阶乘为 %d, sum = %d\n", tmp, ret, sum);

	return 0;
}
