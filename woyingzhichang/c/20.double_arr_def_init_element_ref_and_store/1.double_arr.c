#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i, j;

	//int arr[2][3]={};
	//int arr[2][3] = {11,22,33,44,55,66};
	//int arr[2][3] = {{11,22,33},{77,88,99}};
	//int arr[2][3] = {{11},{77,88,99}};//部分初始化
	//在定义的时候行号可以省略!根据初始化的情况确定
	int arr[][3] ={11,22,33,44,55,66};

	//for (i = 0; i < 2; i++) {
	//	for (j = 0; j < 3; j++) {
	//		scanf("%d", &arr[i][j]);
	//	}
	//}


	printf("arr -> %p\n", arr);
	printf("arr+1 -> %p\n", arr+1);

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			printf("%p --> ", &arr[i][j]);
			printf("%d \n", arr[i][j]);
		}
	}

	return 0;
}
