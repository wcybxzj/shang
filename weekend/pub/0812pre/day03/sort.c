#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	int arr[10] = {};
	int i, j;
	int tmp;

	//数组赋值
	for (i = 0; i < 10; i++) {
		arr[i] = rand() % 100;
	}

	//排序
	for (i = 0; i < 10-1; i++) {//比较趟数
		for (j = 0; j < 10-1-i; j++) { //比较的数组下标
			if (arr[j] > arr[j+1]) {
				tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;	
			}
		}
	}
	//遍历
	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
