#include <stdio.h>

int main(void)
{
	int arr[] = {1,3,5,7,9,11,13,15};
	int *p = arr;

	printf("%d\n", *p++);
	printf("%d\n", *(p++));
	printf("%d\n", ++*p);
	printf("%d\n", *++p);
	printf("%d\n", *(++p));

	// 1 3 6 7 9
	// 1 5 6 7 9
	// 1 3 6 7 9
	// 1 9 3 7 7
	return 0;
}
