#include <stdio.h>

int main(void)
{
	int var1, var2;
	int ret;
	
	printf("input:\n");
	ret = scanf("%d%d", &var1, &var2);

	printf("var is %d\n", var1);
	printf("ret is %d\n", ret);

	return 0;
}
