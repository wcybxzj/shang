#include <stdio.h>

int main(void)
{
	int var;
	char num;
	int ret;

	printf("input\n");
	ret = scanf("%d %c", &var, &num);	
	printf("ret = %d,var = %d, num = %c\n", ret, var, num);

	return 0;
}
