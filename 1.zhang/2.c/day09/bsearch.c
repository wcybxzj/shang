#include <stdio.h>
#include <stdlib.h>

int func(const void * a, const void *b)
{
	const int * a1 = a;
	const int * b1 = b;
	return *a1 - *b1;
}

int main(int argc, const char *argv[])
{
	int arr[10] = {11,22,33,44,55,66,77,88,99,100};
	int search = 77;
	int *p = NULL;
	p = bsearch(&search, arr, 10, sizeof(int), func);
	if (p) {
		printf("found %d\n", *p);
	}else{
		printf("not found %d\n", search);
	}

	return 0;
}
