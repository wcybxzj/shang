#include <stdio.h>

int main(void)
{
	int arr[] = {3,8,2,1,4,9,7,6,5,10};
	int i, j;
	int tmp;

	//2 5 3 
	//0 1 2
	for (i = 1; i < 10; i++) {
		tmp = arr[i];
		for (j = i; j > 0; j--) { //1
			if (tmp < arr[j-1]) {
				arr[j] = arr[j-1];
			} else {
				break;
			}	
		}	
		arr[j] = tmp;
	}

	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
