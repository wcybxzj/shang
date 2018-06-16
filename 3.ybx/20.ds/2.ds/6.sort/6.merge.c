#include <stdio.h>
#include <stdlib.h>

void merging(int arr1[], int num1, int arr2[], int num2)
{
	int i=0, j=0, k=0, m=0;
	int tmp[100]={0};

	while (i < num1 && j < num2) {
		if (arr1[i] < arr2[j]) {
			tmp[k++] = arr1[i++];
		}else{
			tmp[k++] = arr2[j++];
		}
	}
	while (i < num1) {
		tmp[k++] = arr1[i++];
	}
	while (j < num2) {
		tmp[k++] = arr2[j++];
	}
	for (m = 0; m < k; m++) {
		arr1[m] = tmp[m];
	}

}

//二路归并排序递归版:
void merge_sort_recursive(int arr[], int num)
{
	if (num < 2) {
		return;
	}
	int count = num/2;
	int *arr1 = arr;
	int arr1_num = count;
	int *arr2 = arr+count;
	int arr2_num = num-count;
	merge_sort_recursive(arr1, arr1_num);
	merge_sort_recursive(arr2, arr2_num);
	merging(arr1, arr1_num, arr2, arr2_num);
}

static int min(int x, int y) {
	return x < y ? x : y;
}

void merge_sort_while(int arr[], int num) {
	int i, k; int *tmp;
	int *arr1 = arr;
	int *arr2 = malloc(num * sizeof(int));
	int seg, start;
	int low, mid, high;
	int start1, end1;
	int start2, end2;
	for (seg = 1; seg < num; seg*=2) {
		for (start = 0; start < num ; start += seg*2) {
			k = start;
			low = start; mid = min(start+seg, num); high = min(start+seg+seg, num);
			start1 = low; end1 = mid;
			start2 = mid; end2 = high;
			while (start1 < end1 && start2 < end2) {
				arr2[k++] = (arr1[start1]<arr1[start2]) ? arr1[start1++] : arr1[start2++];
			}
			while (start1 < end1) {
				arr2[k++] = arr1[start1++];
			}
			while (start2 < end2) {
				arr2[k++] = arr1[start2++];
			}
		}
		tmp  = arr1;
		arr1 = arr2;
		arr2 = tmp;
	}

	if (arr1!=arr) {
		for (i = 0; i < num; i++) {
			arr2[i] =arr1[i];
		}
		arr2=arr1;
	}

	free(arr2);
}
