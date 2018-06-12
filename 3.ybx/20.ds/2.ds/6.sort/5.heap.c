#include "5.heap.h"


static int compare_times=0;//比较次数
static int move_times=0;//移动次数

void heap_adjust1 (int arr[], int start ,int num)
{
	int i, tmp;
	tmp = arr[start];
	for (i = 2*start; i <= num ; i*=2) {
		if (i<num && arr[i]<arr[i+1]) {
			i++;
		}
		if (tmp>=arr[i]) {
			break;
		}
		arr[start] = arr[i];
		start = i;
	}
	arr[start] = tmp;
}

void heap_adjust2 (int arr[], int start ,int num)
{
	int i, tmp;
	tmp = arr[start];
	for (i = 2*start+1; i < num ; i=2*i+1) {
		compare_times++;
		if (i+1 < num && arr[i]<arr[i+1]) {
			i++;
		}
		if (tmp>=arr[i]) {
			break;
		}
		arr[start] = arr[i];
		move_times++;
		start = i;
	}
	arr[start] = tmp;
	move_times++;
}


//版本2:数据从下标1开始
void heap_sort1(int arr[], int num)
{
	int i;
	//初始化，i從最後一個父節點開始調整
	for (i = num/2; i > 0 ; i--) {
		heap_adjust1(arr, i, num);
	}

	for (i = num; i > 1 ; i--) {
		swap(&arr[1], &arr[i]);
		heap_adjust1(arr, 1, i-1);
	}
}

//版本2:数据从下标0开始
void heap_sort2(int arr[], int num)
{
	compare_times=0;//比较次数
	move_times=0;//移动次数
	int i;
	//初始化，i從最後一個父節點開始調整
	for (i = num/2-1; i >= 0 ; i--) {
		heap_adjust2(arr, i, num);
	}

	for (i = num-1; i > 0 ; i--) {
		swap(&arr[0], &arr[i]);
		move_times+=3;
		heap_adjust2(arr, 0, i);
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}

