#include <stdio.h>

#define NUM 8

int getMin(int arr[], int i)
{
	int j, k, min;
	k = i;
	min  = arr[i];
	for (j = i+1; j < NUM; j++) {
		if (arr[j] < min) {
			k = j;
			min = arr[j];
		}
	}
	return k;
}

void simple_select(int arr[])
{
	int i, j;
	for (i = 0; i < NUM; i++) {
		j = getMin(arr, i);
		if (i != j) {
			arr[i] = arr[i] ^ arr[j];
			arr[j] = arr[i] ^ arr[j];
			arr[i] = arr[i] ^ arr[j];
		}
	}
}

int main(int argc, const char *argv[])
{
	int i;
	int arr[NUM] = {};
	srand(time(NULL));

	for (i = 0; i < NUM; i++) {
		arr[i] = rand()%101;
	}

	simple_select(arr);

	for (i = 0; i < NUM; i++) {
		printf("%d\n", arr[i]);
	}

	return 0;
}
