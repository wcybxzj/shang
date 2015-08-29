#include <stdio.h>

int main(void)
{
	int var1 = 1, var2 = 1;
	int ret;
	int n, i;

	printf("input:");
	scanf("%d", &n);

	printf("%d %d ", var1, var2);
	for (i = 3; i <= n; i ++) {
		ret = var1 + var2;
		var1 = var2;
		var2 = ret;
		printf("%d ", ret);
	}
	printf("\n");
	printf("febnaci 第%d项是%d\n", n, ret);

	return 0;
}
