#include <stdio.h>
int main(int argc, const char *argv[])
{
	int arr[2][3]={ 11,22,33,44,55,66};
	printf("%d\n", **arr);//11
	printf("%d\n", *(*(arr+1)));//44
	printf("%d\n", *(*(arr+1)+1));//55

	printf("%d\n", **(&arr+1));// 未初始化的值
	printf("start address %d\n", arr);
	printf("start address %d\n", &arr);
	printf("start address %d\n", &arr+1);

	return 0;
}
