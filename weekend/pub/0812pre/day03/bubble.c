#include <stdio.h>

int main(void)
{
	int arr[] = {4,3,1,8,5,7,6,9,2};	
	int i, j;
	int tmp;

	for (i = 0; i < 8; i++) {
		for (j = 8; j > i; j--) {
			if (arr[j] > arr[j-1]) {
				tmp = arr[j];
				arr[j] = arr[j-1];
				arr[j-1] = tmp;	
			}
		}	
	}

	for (i = 0; i < 9; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
