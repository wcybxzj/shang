#include <stdio.h>

int main(void)
{
	int m, n, r;
	int var0, var1;

	printf("input two numbers: ");
	scanf("%d %d", &m, &n);

	var0 = m;var1 = n;
	if (m < n) {
		m = m ^ n;
		n = m ^ n;
		m = m ^ n;
	}

	while ((r = m % n) != 0) {
		m = n;
		n = r;
	}
	printf("the max is %d\n", n);
	printf("the min is %d\n", (var0 * var1) / n);

	return 0;
}
