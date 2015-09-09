#include <stdio.h>

typedef int a;

typedef int (*p_t)(int);

int factor(int n)
{
#if 0
	printf("factor: %p\n", factor);
	printf("*factor: %p\n", *factor);
	printf("&factor: %p\n", &factor);
#endif
	int ret = 1;	

	while (n){ 
		ret *= n;
		n--;
	}
	return ret;
}

int main(void)
{
	a b = 10;
	p_t q = factor;

	printf("%d, b = %d\n", factor(5), b);	

	int (*p)(int) = &factor;
	printf("%d\n", (*p)(6));
	printf("%d\n", q(6));

	return 0;
}
