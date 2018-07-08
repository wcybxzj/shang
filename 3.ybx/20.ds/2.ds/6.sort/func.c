#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


void my_srand()
{
	srand((int)time(0));
}

//取值min->max范围内的数
int my_rand(int min, int max)
{
	return (rand()%(max-min+1))+min;
}

void travel(int arr[], int num)
{
	printf("遍历:");
	int i;
	for (i = 0; i < num; i++) {
		printf("%d ",arr[i]);
	}
	printf("\n");
}

void swap(int *x, int *y) {
    int t = *x;
    *x = *y;
    *y = t;
}

void copy_arr(int dst_arr[], int src_arr[], int num)
{
	int i;
	for (i = 0; i < num; i++) {
		dst_arr[i] =src_arr[i];
	}
}
