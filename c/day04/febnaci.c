#include <stdio.h>

int main(void)
{
	int arr[20] = {1,1};			
	int i;

	for (i = 2; i < 20; i++) {
		arr[i] = arr[i-1] + arr[i-2];
	}

	for (i = 0; i < 20 || !printf("\n"); i++) {
		printf("%d ", arr[i]);
	}

	return 0;
}
