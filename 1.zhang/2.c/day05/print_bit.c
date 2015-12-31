#include <stdio.h>

int number_of(void)
{
	int cnt = 0;	
	unsigned int tmp = ~0u;

	while (tmp >> cnt++  != 0x1);

	return cnt;
}

void binery_pri(unsigned num)
{
	int i;	

	for (i = number_of() - 1; i >= 0; i--) {
		(num >> i) & 0x1 ? putchar('1'):putchar('0');
		if (i % 8 == 0) {
			putchar(' ');
		}
	}
	printf("\n");
}

int main(void)
{
	unsigned int var;

#if 1
	printf("input:");
	scanf("%d", &var);

	binery_pri(var);
#endif
	return 0;
}
