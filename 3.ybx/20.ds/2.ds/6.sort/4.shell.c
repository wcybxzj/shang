#include "4.shell.h"

//初始化用len/2为步长,直到步长为1
void shell_sort1(int arr[], int len) {
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int gap, i, j;
	int tmp;
	for (gap = len >> 1; gap > 0; gap >>= 1){
		//printf("gap:%d:\n", gap);
		for (i = gap; i < len; i++) {
			//printf("-i:%d",i);
			tmp = arr[i];
			for (j=i-gap; j>=0; j-=gap) {
				//printf("-j:%d",j);
				compare_times++;
				if (arr[j] > tmp) {
					arr[j+gap]=arr[j];
					move_times++;
				}else{
					break;
				}
			}
			//printf("\n");
			arr[j+gap]=tmp;
			move_times++;
		}
		//printf("\n");
	}
	printf("比较次数:%d 移动次数:%d 总数:%d\n",
			compare_times, move_times,compare_times+move_times);
}
