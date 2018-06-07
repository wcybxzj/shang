#include "func.h"
#include <time.h>
#define  NUM 20

//冒泡优化版本vs 选择排序
//数据:完全一样的没有特殊性的随机数据
//比较次数:冒泡排序和选择排序基本一样
//移动次数:选择排序的移动次数远远少于冒泡排序

/*
正宗冒泡排序优化版:
遍历:93 37 49 99 33 35 5 19 54 11 86 5 85 9 59 56 8 36 95 28
比较次数:180 移动次数:104
遍历:5 5 8 9 11 19 28 33 35 36 37 49 54 56 59 85 86 93 95 99
选择排序:
遍历:93 37 49 99 33 35 5 19 54 11 86 5 85 9 59 56 8 36 95 28
比较次数:190 移动次数:16
遍历:5 5 8 9 11 19 28 33 35 36 37 49 54 56 59 85 86 93 95 99
*/
void test2_bublle_vs_select()
{
	int i;
	int arr1[NUM] = {};
	int arr2[NUM] = {};
	srand(time(NULL));

	for (i = 0; i < NUM; i++) {
		arr2[i] = arr1[i] = rand()%101;
	}

	printf("正宗冒泡排序优化版:\n");
	travel(arr2, NUM);
	bubble_version2(arr2, NUM);
	travel(arr2, NUM);

	printf("选择排序:\n");
	travel(arr1, NUM);
	select_sort(arr1, NUM);
	travel(arr1, NUM);
}

//冒泡优化版本vs 选择排序
//数据:右边大量的数据已经是有序的
//比较次数:冒泡优化版的比较次数更少
//移动次数:冒泡排序和选择排序基本一样
/*
正宗冒泡排序优化版:
遍历:32 32 54 12 52 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
比较次数:70 移动次数:4
遍历:12 32 32 52 54 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
选择排序:
遍历:32 32 54 12 52 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
比较次数:190 移动次数:3
遍历:12 32 32 52 54 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219
*/
void test3_bublle_vs_select()
{
	int i;
	int arr1[NUM] = {};
	int arr2[NUM] = {};

	for (i = 0; i < NUM; i++) {
		arr2[i] = arr1[i] = rand()%101;
	}

	for (i = 5; i < NUM; i++) {
		arr2[i] = arr1[i] = 200+i;
	}

	printf("正宗冒泡排序优化版:\n");
	travel(arr2, NUM);
	bubble_version2(arr2, NUM);
	travel(arr2, NUM);

	printf("选择排序:\n");
	travel(arr1, NUM);
	select_sort(arr1, NUM);
	travel(arr1, NUM);
}

int main(int argc, const char *argv[])
{
	test2_bublle_vs_select();
	//test3_bublle_vs_select();
	return 0;
}
