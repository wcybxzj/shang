#include "2.select.h"

void select_sort(int *arr, int num)
{
	int compare_times=0, move_times=0;
	int i, j, min_index;
	int tmp;
	for (i = 0; i < num-1; i++) {
		min_index = i;
		for (j = i+1; j < num; j++) {
			if (arr[min_index] > arr[j]) {
				min_index = j;
			}
			compare_times++;
		}
		if (min_index != i ) {
			tmp = arr[i];
			arr[i] = arr[min_index];
			arr[min_index] = tmp;
			move_times=move_times+3;
		}
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}
