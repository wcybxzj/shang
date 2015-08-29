#include <stdio.h>

int main(void)
{
	int arr[10] = {7,3,1,6,8,5,4,2,9,10};
	int i, j, k;
	int tmp;

	for (i = 0; i < 9; i++) {
		tmp = arr[i];
		k = i;
		for (j = i+1; j < 10; j++) {
			if (arr[j] > tmp) {
				tmp = arr[j];
				k = j;
			}
		}	
		arr[k] = arr[i];
		arr[i] = tmp;
	}

	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
