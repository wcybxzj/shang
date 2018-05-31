#include <stdio.h>

#if 0
排序算法：
	冒泡、选择、直接插入、快速排序、希尔排序、归并排序、
	堆排序、桶排序
	从下到大
#endif

#define NUM 20 

void swap(int *p, int m, int n)
{
	int tmp;

	tmp = p[m];
	p[m] = p[n];
	p[n] = tmp;
}

void bublle(int p[], int len)
{
	int i, j;

	for (i = 0; i < len-1; i++) {
		for (j = 0; j < len-i-1; j++) {
			if (p[j] > p[j+1]) {
				swap(p, j, j+1);
			}	
		}
	}	
}

void select(int *p, int len)
{
	int i, j;
	int tmp, k;

	for (i = 0; i < len - 1; i++) {
		tmp = p[i];
		k = i;
		for (j = i+1; j < len; j++) {
			if (tmp > p[j]) {
				tmp = p[j];
				k = j;
			}	
		}	
		p[k] = p[i];
		p[i] = tmp;
	}	
}

void insert(int *p, int len)
{
	int i, j;
	int tmp;
// 1 2 3 4 5 8 7 
	for (i = 1; i < len; i++) {
		tmp = p[i];
		for (j = i-1; j >= 0; j--) {
			if (tmp < p[j]) {
				p[j+1] = p[j];
			} else {
				break;
			}
		}	
		p[j+1] = tmp;
	}
}

void traval(int *p, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		printf("%d ", p[i]);
	}
	printf("\n");
}

int main(void)
{
	srand(time(NULL));
	int arr[NUM] = {};
	int i;

	for (i = 0; i < NUM; i++) {
		arr[i] = rand() % 100;
	}
	traval(arr, NUM);
	insert(arr, NUM);
	traval(arr, NUM);

	return 0;
}

