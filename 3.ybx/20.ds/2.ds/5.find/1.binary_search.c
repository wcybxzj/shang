#include <stdio.h>
#include <stdlib.h>

int bin_search_while(int arr[], int num, int find)
{
	int low, mid, high;
	low = 0;
	high = num-1;
	while (low<=high) {
		mid = low+((high-low)/2);
		if (arr[mid] < find) {
			low=mid+1;
		}else if(arr[mid]<find){
			high=mid-1;
		}else{
			return mid;
		}
	}
	return -1;
}

int inner_func(int arr[], int low, int high, int find)
{
	int mid;
	while (low<=high) {
		mid = low+((high-low)/2);
		if (arr[mid] < find) {
			low=mid+1;
			return inner_func(arr,low, high, find);
		}else if(arr[mid]<find){
			high=mid-1;
			return inner_func(arr,low, high, find);
		}else{
			return mid;
		}
	}
	return -1;
}

int bin_search_recursion(int arr[], int num, int find)
{
	int low, high;
	low = 0;
	high = num-1;
	return inner_func(arr, low, high, find);
}

void travel(int arr[], int num)
{
	int i;
	for (i = 0; i < num; i++) {
		printf("%d ",arr[i]);
	}
	printf("\n");
}

void test()
{
	int i, j;
	int num=10;
	int *arr1 = malloc(num*sizeof(int));
	int *arr2 = malloc(num*sizeof(int));
	for (i = 0; i < num; i++) {
		arr1[i]=10+i;
		arr2[i]=10+i+2;
	}

	printf("arr1:\n"); travel(arr1, num);
	printf("arr2\n"); travel(arr2, num);

	printf("==========循环二分查找测试1:=============\n");
	for (i = 0; i < num; i++) {
		j = bin_search_while(arr1, num, arr1[i]);
		printf("try find %d ", arr1[i]);
		if (j==-1) {
			printf("not find %d\n",arr1[i]);
		}else{
			printf("find %d\n", arr2[i]);
		}
	}

	printf("==========循环二分查找测试2:=============\n");
	for (i = 0; i < num; i++) {
		j = bin_search_while(arr1, num, arr2[i]);
		printf("try find %d ", arr2[i]);
		if (j==-1) {
			printf("not find %d\n",arr2[i]);
		}else{
			printf("find %d\n", arr2[i]);
		}
	}

	printf("==========递归二分查找测试1:=============\n");
	for (i = 0; i < num; i++) {
		j =bin_search_recursion(arr1, num, arr1[i]);
		printf("try find %d ", arr1[i]);
		if (j==-1) {
			printf("not find %d\n",arr1[i]);
		}else{
			printf("find %d\n", arr2[i]);
		}
	}

	printf("==========递归二分查找测试2:=============\n");
	for (i = 0; i < num; i++) {
		j = bin_search_recursion(arr1, num, arr2[i]);
		printf("try find %d ", arr2[i]);
		if (j==-1) {
			printf("not find %d\n",arr2[i]);
		}else{
			printf("find %d\n", arr2[i]);
		}
	}
}

int main(int argc, const char *argv[])
{
	test();
	return 0;
}
