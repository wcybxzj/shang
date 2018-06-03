#include <stdio.h>
#include <stdlib.h>

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
				move_times++;
			}
			compare_times++;
		}
	}
	printf("比较次数:%d 移动次数:%d\n",compare_times, move_times);
}

//普通版:
//返回值是运行次数
void bubble_version1(int arr[], int num)
{
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int i, j, tmp;
	for (i = 0; i < num; i++) {
		for (j = 0; j < num-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
				compare_times++;
			}
			compare_times++;
		}
	}
	printf("比较次数:%d 移动次数:%d\n",compare_times, move_times);
}

//优化版:若某一趟排序中未进行一次交换，则排序结束
//返回值是运行次数
void bubble_version2(int arr[], int num)
{
	int compare_times=0;//比较次数
	int move_times=0;//移动次数
	int flag=0;
	int i, j, tmp;
	for (i = 0; i < num; i++) {
		flag=0;
		for (j = 0; j < num-i-1; j++) {
			if (arr[j] > arr[j+1]) {
				tmp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = tmp;
				flag=1;
				move_times++;
			}
			compare_times++;
		}
		if (flag==0) {
			break;
		}
	}
	printf("比较次数:%d 移动次数:%d\n",compare_times, move_times);
}

void travel(int arr[], int num)
{
	printf("遍历:\n");
	int i;
	for (i = 0; i < num; i++) {
		printf("%d ",arr[i]);
	}
	printf("\n");
}

//测试版本1和版本2,但是有因为数据的原因没体现出版本2的优势
void test2()
{
	int i=0;
	int num=0;
	int run_time=0;
	int arr[]  = {13, 15, 20, 25, 24, 10, 16, 21, 23, 4, 11, 22};
	int arr2[] = {13, 15, 20, 25, 24, 10, 16, 21, 23, 4, 11, 22};

	num =sizeof(arr)/sizeof(*arr);
	bubble_version1(arr, num);
	travel(arr, num);

	bubble_version2(arr2, num);
	travel(arr2, num);
}

//3个版本的测试
void test1()
{
	int i=0;
	int num=0;
	int arr2[]  = {100,11,22,33,44,55,66,77,88,99};
	int arr3[]  = {100,11,22,33,44,55,66,77,88,99};
	int arr4[]  = {100,11,22,33,44,55,66,77,88,99};
	num =sizeof(arr2)/sizeof(*arr2);

	bubble_version0(arr2, num);
	travel(arr2, num);

	bubble_version1(arr3, num);
	travel(arr3, num);

	bubble_version2(arr4, num);
	travel(arr4, num);
}

int main(int argc, const char *argv[])
{
	test1();
	test2();
	return 0;
}
