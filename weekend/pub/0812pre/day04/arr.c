#include <stdio.h>

int main(void)
{
	int arr[10] = {};
	int i;

	for (i = 0; i < 10; i++) {
		arr[i] = 10-i;
	}

	for (i = 0; i < 10; i ++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
