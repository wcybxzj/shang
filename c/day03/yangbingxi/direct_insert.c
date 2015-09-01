#include <stdio.h>

#define NUM 8

void direct_insert(int arr[])
{
	int i, j, tmp;
	for (i = 2; i <= NUM; i++) {
		if (arr[i] < arr[i-1]) {
			arr[0] = arr[i];
			for (j = i-1; arr[0] < arr[j]; j--) {
				arr[j+1] = arr[j];
			}
			arr[j+1] = arr[0];
		}
	}
}

int main(int argc, const char *argv[])
{
	int i;
	int arr[NUM] = {};
	srand(time(NULL));

	for (i = 1; i < NUM; i++) {
		arr[i] = rand()%101;
	}

	direct_insert(arr);

	for (i = 1; i < NUM; i++) {
		printf("%d\n", arr[i]);
	}

	return 0;
}
