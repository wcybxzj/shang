#include <stdio.h>

int test(int *b) 
{
	(*b)++;
	
	return *b;
}

int main(void)
{
	int (*p)(int *);
	int m = 10;

	p = &test;
	
	printf("test = %p\n", test);
	printf("&test = %p\n", &test);
	printf("*test = %p\n", *test);

	printf("%d\n", (*p)(&m));
	printf("%d\n", (*test)(&m));
	printf("%d\n", (&test)(&m));

	return 0;
}
