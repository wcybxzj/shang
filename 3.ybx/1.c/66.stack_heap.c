#include <stdio.h>
#include <stdlib.h>

int main(){
	//栈空间递减，堆空间递增
	int a;
	int b;
	printf("%p\n", &a);//0x7fffea0411e4
	printf("%p\n", &b);//0x7fffea0411e0
	int *ptr1, *ptr2;
	ptr1 = malloc(sizeof(long long));
	ptr2 = malloc(sizeof(long long));
	printf("%p\n", &ptr1);//0x7fffea0411d8
	printf("%p\n", &ptr2);//0x7fffea0411d0
	printf("%p\n", ptr1); //0xa32010
	printf("%p\n", ptr2); //0xa32030

	//所有数组的空间都是递增
	int arr[2];
	printf("%p\n", &arr[0]);// 0x7fffea0411c0
	printf("%p\n", &arr[1]);// 0x7fffea0411c4
	int *arr2;
	arr2 = malloc(sizeof(int)*2);
	printf("%p\n", &arr2[0]);//0xa32050
	printf("%p\n", &arr2[1]);//0xa32054



	//printf("%d\n", sizeof(void *));//8
	return 0; 
}
