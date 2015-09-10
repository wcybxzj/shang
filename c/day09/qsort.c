#include <stdio.h>
#include <stdlib.h>

void traval(int arr[], int num);
int cmpare(const void *d1, const void *d2);

int binsearch(int *arr, int num, int key)
{
	int start, end;
	int mid;
	
	start = 0, end = num - 1;	

	while (start <= end) {
		mid = (start + end) / 2;	
		if (arr[mid] == key) {
			return mid;
		}
		if (arr[mid] > key) {
			end = mid - 1;
		} else {
			start = mid + 1;
		}
	}

	return -1;
}

int main(void)
{
	int arr[] = {5,4,3,1,9,2,10,7,8,6};
	int id, ind;
	
	traval(arr, 10);
	qsort(arr, 10, sizeof(int), cmpare);
	traval(arr, 10);

	id = 7;

	ind = binsearch(arr, 10, id);
	if (ind < 0) {
		printf("not found\n");
	}else {
		printf("find it in %d\n", ind);
	}
		
	return 0;
}

int cmpare(const void *d1, const void *d2)
{
	const int *num1 = d1;	
	const int *num2 = d2;	

	return *num1 - *num2;
}

void traval(int arr[], int num)
{
	int i;

	for (i = 0; i < num || !printf("\n"); i++) {
		printf("%d ", arr[i]);
	}
}
