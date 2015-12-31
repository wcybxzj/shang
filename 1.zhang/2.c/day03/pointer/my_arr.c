#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	int arr[10];//自动变量内容为随机值
	int arr2[10]={};//所有数组内容初始化为0
	int arr3[10]={1, 2};//1,2,0,0,0,0,0...
	int arr4[10]={1, 2, [8]=9, [3]=12};//1,2,0,12....9
	int arr5[10]={[8]=9, [3]=12};//0 0 0 12 ....9..

	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	for (i = 0; i < 10; i++) {
		printf("%d ", arr2[i]);
	}
	printf("\n");

	for (i = 0; i < 10; i++) {
		printf("%d ", arr3[i]);
	}
	printf("\n");

	for (i = 0; i < 10; i++) {
		printf("%d ", arr4[i]);
	}
	printf("\n");

	for (i = 0; i < 10; i++) {
		printf("%d ", arr5[i]);
	}
	printf("\n");

	printf("====================\n");
	return 0;
}
