#include <stdio.h>

#if 0
	数组：
		相同类型数据元素的集合。
	type arr_name[element_num];
	arr_name:常量，数组的首地址
	arr_name[0] ~ arr_name[num-1];
#endif

int main(void)
{
	int arr[10] = {1,[8]=9, [3]=12};
//	int arr[10];
	int i;

	for (i = 0; i < 10; i++) {
		printf("arr[%d] = %d\n", i, arr[i]);
	}

	printf("sizeof(arr) = %d\n", sizeof(arr));
	printf("sizeof(arr[0]) = %d\n", sizeof(arr[0]));
	printf("*(arr+1) = %d\n", *(arr+1));
	printf("--------------------\n");
	printf("arr 	= %p\n", arr);
	printf("&arr[0] = %p\n", &arr[0]);
	printf("&arr 	= %p\n", &arr);
	printf("&arr+1  = %p\n", &arr + 1);
	
	return 0;
}
