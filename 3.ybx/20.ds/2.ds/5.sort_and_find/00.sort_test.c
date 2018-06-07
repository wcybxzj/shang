#include "func.h"
#include <time.h>

void heap_test1(int n);


//冒泡优化版本vs 选择排序vs插入排序
//数据:完全一样的没有特殊性的随机数据

/*
正宗冒泡排序优化版:
比较次数:187 移动次数:111
遍历:8 16 20 24 32 37 44 45 50 63 64 71 76 78 89 94 96 97 97 97 

选择排序:
比较次数:190 移动次数:16
遍历:8 16 20 24 32 37 44 45 50 63 64 71 76 78 89 94 96 97 97 97 

插入排序:
比较次数:128 移动次数:111
遍历:8 16 20 24 32 37 44 45 50 63 64 71 76 78 89 94 96 97 97 97 
*/
void test(int n)
{
	int i,num;
	srand(time(NULL));

	if (n==0) {
		num=my_rand(1,25);
		//num=my_rand(1,5);
	}else{
		num = n;
	}

	printf("数组长度:%d\n", num);
	int *arr1=calloc(num, sizeof(int));
	int *arr2=calloc(num, sizeof(int));

	for (i = 0; i < num; i++) {
		arr1[i] = arr2[i] = rand()%101;
	}

	printf("正宗冒泡排序优化版:\n");
	travel(arr1, num);
	bubble_version2(arr1, num);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);

	printf("选择排序:\n");
	travel(arr1, num);
	select_sort(arr1, num);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);

	printf("插入排序1:\n");
	travel(arr1, num);
	insert_sort1(arr1, num);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);

	printf("希尔排序1:\n");
	travel(arr1, num);
	shell_sort1(arr1, num);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);

	//heap_test1(n);

	printf("堆排序2:\n");
	travel(arr1, num);
	heap_sort2(arr1, num);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);
}

void heap_test1(int n)
{
	int i,num;
	srand(time(NULL));
	if (n==0) {
		num=my_rand(1,25);
	}else{
		num = n;
	}
	int *arr1=calloc(num, sizeof(int));
	int *arr2=calloc(num, sizeof(int));
	arr1[0] = arr2[0] = -1;
	for (i = 1; i < num; i++) {
		arr1[i] = arr2[i] = rand()%101;
	}
	printf("堆排序1:\n");
	travel(arr1, num);
	heap_sort1(arr1, num-1);
	travel(arr1, num);
	copy_arr(arr1, arr2, num);
}

int main(int argc, const char *argv[])
{
	int  n=0;
	if (argc == 2) {
		n = atoi(argv[1]);
	}
	test(n);


	return 0;
}
