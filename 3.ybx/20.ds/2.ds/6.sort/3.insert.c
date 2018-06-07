#include "3.insert.h"

void insert_sort1(int *arr, int num){
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int i,j,tmp;
	for (i = 1; i < num; i++) {
		tmp = arr[i];
		for (j=i-1; j>=0; j--) {
			compare_times++;
			if (arr[j] > tmp) {
				arr[j+1]=arr[j];
				move_times++;
			}else{
				break;
			}
		}
		arr[j+1]=tmp;
		move_times++;
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}
