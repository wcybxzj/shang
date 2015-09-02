#include <stdio.h>

int main(void)
{
	int arr[] = {1, 2, 3};		
	char i;
	int *p = arr;
	char *q = &i;

	printf("sizeof(p) = %d\n", sizeof(p));	
	printf("sizeof(arr) = %d\n", sizeof(arr));
	printf("sizeof(char *) = %d\n", sizeof(char *));
	printf("sizeof(int *) = %d\n", sizeof(int *));
	printf("sizeof(long *) = %d\n", sizeof(long *));
	printf("sizeof(float *) = %d\n", sizeof(float *));
	printf("sizeof(double *) = %d\n", sizeof(double *));

	printf("p = %p\n", p);
	printf("p = %p\n", p+1);
	printf("q = %p\n", q);
	printf("q = %p\n", q+1);

	return 0;
}
