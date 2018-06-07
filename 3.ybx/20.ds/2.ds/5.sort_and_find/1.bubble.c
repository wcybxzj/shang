#include "1.bubble.h"

//冒泡定义:两两相邻记录,如果反序则交换,直到没有反序的记录为止

//冒泡错误版:
//这里的错误说的不是排序功能错误，
//而是这种写法不符合冒泡排序的定义,不是亮亮相邻记录进行比较
void bubble_version0(int arr[],int num)
{
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int i=0, j=0, tmp=0;
	for (i = 0; i < num-1; i++) {
		for (j = i+1; j < num; j++) {
			if (arr[i]>arr[j]) {
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
				move_times=move_times+3;
			}
			compare_times++;
		}
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}

//冒泡正确版:
//返回值是运行次数
void bubble_version1(int arr[], int num)
{
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int i, j, tmp;
	for (i = 0; i < num-1; i++) {
		for (j = 0; j < num-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
				move_times=move_times+3;
			}
			compare_times++;
		}
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}

//冒泡正确优化版:
//若某一趟排序中未进行一次交换，则排序结束
//返回值是运行次数
void bubble_version2(int arr[], int num)
{
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int flag=0;
	int i, j, tmp;
	for (i = 0; i < num-1; i++) {
		flag=0;
		for (j = 0; j < num-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
				flag=1;
				move_times=move_times+3;
			}
			compare_times++;
		}
		if (flag==0) {
			break;
		}
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}
