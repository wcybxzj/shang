#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	int arr[10];
	int i;
	int sum = 0;

	for (i = 0; i < 10; i++) {
		arr[i] = rand()	% 100;
		printf("%d ", arr[i]);
		sum += arr[i];
	}
	printf("\n");

	printf("sum = %d\n", sum);

	return 0;
}
