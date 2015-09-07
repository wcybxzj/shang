#include <stdio.h>
#define NUM 10

//in mac os
//gcc quick.c -std=c89
void swap(int *p, int m, int n)
{
	int tmp;
	tmp = p[m];
	p[m] = p[n];
	p[n] = tmp;
}

int partition(int arr[], int low, int high){
	int p = arr[low];
	while (low < high) {
		while (low < high && arr[high] >= p) {
			high--;
		}
		arr[low] = arr[high];
		while (low < high && arr[low] <= p) {
			low++;
		}
		arr[high] = arr[low];
	}

	arr[low] = p;
	return low;
}

void quick(int arr[], int low, int high){
	int pivotloc;
	if (low < high) {
		pivotloc = partition(arr, low, high);
		quick(arr, low, pivotloc-1);
		quick(arr, pivotloc+1, high);
	}
}

void traval(int *p)
{
	int i;
	for (i = 0; i < NUM; i++) {
		printf("%d ", p[i]);
	}
	printf("\n");
}

int main(void)
{
	srand(time(NULL));
	int arr[NUM] = {};
	int i;

	for (i = 0; i < NUM; i++) {
		arr[i] = rand() % 100;
	}
	traval(arr);
	quick(arr,0, NUM-1);
	traval(arr);

	return 0;
}

