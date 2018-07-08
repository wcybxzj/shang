#include <stdio.h>

void quick_sort_recursive(int arr[], int start, int end) {
	if (start >= end)
		return;

	//int i = start;
	//for (; i <= end; i++) {
	//	printf("%d ",arr[i]);
	//}
	//printf("\n");

	int mid = arr[end];
	int left = start, right = end - 1;
	while (left < right) {
		while (arr[left] < mid && left < right)
			left++;
		while (arr[right] >= mid && left < right)
			right--;
		swap(&arr[left], &arr[right]);
	}
	if (arr[left] >= arr[end]){
		swap(&arr[left], &arr[end]);
	}
	else{
		left++;
	}
	quick_sort_recursive(arr, start, left - 1);
	quick_sort_recursive(arr, left + 1, end);
}

//快排递归版
void quick_sort_r(int arr[], int len) {
	quick_sort_recursive(arr, 0, len - 1);
}

