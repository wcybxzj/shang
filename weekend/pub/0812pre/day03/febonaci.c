#include <stdio.h>

int main(void)
{
	// 1 1 2 3 5 8....
	int arr[10];
	int i;

	arr[0] = 1;
	arr[1] = 1;

	for (i = 2; i < 10; i++) {
		arr[i] = arr[i-1]+arr[i-2];
	}
	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
