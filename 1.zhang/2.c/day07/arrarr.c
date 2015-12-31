#include <stdio.h>

int main(void)
{
	int arr[] = {1,2,3,4,5,6,7,8,9};
	int i, j;
#if 0
	int arr[3][3] = {
		{1,2,3},
		{4,5,6},
		{7,8,9}
	}
#endif

	for (i = 0; i < 3; i++) {
		for (j = i*3; j < i*3+3; j++) {
			printf("%d ", arr[j]);
		}
		printf("\n");
	}

	return 0;
}
