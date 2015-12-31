#include <stdio.h>
#include <stdlib.h>
#define NUM 10

int f1(const void *a, const void *b)
{
	const int *a1 = a;
	const int *b2 = b;

	if (*a1 > *b2) {
		return 1;
	}else if(*a1==*b2){
		return 0;
	}else{
		return -1;
	}
}

int main(int argc, const char *argv[])
{
	int i;
	int arr[NUM] = {33,22,11,55,77,3,99,2,13,1};
	//int (*fun)(int a, int b);
	qsort(arr, NUM, 4,f1);
	for (i = 0; i < NUM; i++) {
		printf("%d ", arr[i]);
	}

	return 0;
}
