#include <stdio.h>
#define NUM 10

//递归版本
int bin(int arr[], int begin, int end, int search)
{
	int middle = (begin+end)/2;
	//printf("middle %d\n", middle);
	if (begin == end && search != arr[begin]) {
		return -1;
	}
	if (search == arr[middle]) {
		return middle;
	}else if(search > arr[middle]){
		return bin(arr, middle+1, end, search);
	}else{
		return bin(arr,begin, middle-1,search);
	}
}

//循环版本
int bin_while(int arr[], int len, int search){
	int start, end, mid;
	start = 0;
	end = len -1;
	while (start <= end) {
		mid = (start+end)/2;
		printf("mid %d\n", mid);
		if (search == arr[mid]) {
			return mid;
		}else if(search > arr[mid]){
			start = mid + 1;
		}else{
			end  = mid - 1;
		}
	}
	return -1;
}

int main(int argc, const char *argv[])
{
	int re;
	int arr[NUM] = {11,22,33,44,55,66,77,88,99,100};

//	re = bin(arr, 0 , NUM-1, 33);//2
//	printf("%d\n", re);
//
//	re = bin(arr, 0 , NUM-1, 55);//4
//	printf("%d\n", re);
//
//	re = bin(arr, 0 , NUM-1,11);
//	printf("%d\n", re);
//
//	re = bin(arr, 0 , NUM-1,100);
//	printf("%d\n", re);
//
//	re = bin(arr, 0 , NUM-1, 200);
//	printf("%d\n", re);
//
	printf("======================================\n");

	//re = bin_while(arr, NUM, 33);//2
	//printf("%d\n", re);

	//re = bin_while(arr, NUM, 55);//4
	//printf("%d\n", re);

	re = bin_while(arr, NUM,11);
	printf("%d\n", re);

	//re = bin_while(arr, NUM,100);
	//printf("%d\n", re);

	//re = bin_while(arr, NUM, 200);
	//printf("%d\n", re);

	return 0;
}
