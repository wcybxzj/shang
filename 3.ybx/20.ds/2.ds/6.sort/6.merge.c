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

void merge_sort(int arr[], int num)
{
	if (num < 2) {
		return;
	}

	int count = num/2;
	int *arr1 = arr;
	int arr1_num = count;
	int *arr2 = arr+count;
	int arr2_num = num-count;

	merge_sort(arr1, arr1_num);
	merge_sort(arr2, arr2_num);

	merging(arr1, arr1_num, arr2, arr2_num);
}

//二路归并排序递归版:
void merge_recusive(int arr[], int num)
{
	merge_sort(arr, num);
}

