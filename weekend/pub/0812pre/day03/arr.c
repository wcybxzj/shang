#include <stdio.h>

int main(void)
{
	int arr[10];
	//int arr[10] = {1};
	//arr[0]~arr[9];
	int i;
	int *p;
	p = arr;

	for (i = 0; i < 10; i++) {
		arr[i] = i+1;
		printf("arr[%d] = %d ", i, arr[i]);
	}
	printf("\n");
	printf("arr = %p\n", arr);
	printf("&arr[0] = %p\n", &arr[0]);
	printf("p = %p\n", p);
	printf("&arr = %p\n", &arr);
	printf("&arr+1 = %p\n", &arr+1);

	printf("\n");

	return 0;
}
