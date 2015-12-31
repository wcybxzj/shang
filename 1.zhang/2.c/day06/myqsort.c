#include <stdio.h>

#define SIZE 20

void print_arr(int *p, int len);
void creat_arr(int *p, int len);
int partation(int *p, int low, int high);
void _Qsort(int *p, int low, int high);
void Qsort(int *p, int len);

int main(void)
{
	srand(time(NULL));
	int arr[SIZE] = {};
		
	creat_arr(arr, SIZE);
	print_arr(arr, SIZE);
	Qsort(arr, SIZE);
	print_arr(arr, SIZE);

	return 0;
}

void creat_arr(int *p, int len)
{
	int i;
	
	for (i = 0; i < len; i++) {
		p[i] = rand() % 100;
	}
}

void print_arr(int *p, int len)
{
	int i;

	for (i = 0; i < len || !printf("\n"); i++) {
		printf("%d ", p[i]);
	}
}

void Qsort(int *p, int len)
{
	_Qsort(p, 0, len-1);
}

void _Qsort(int *p, int low, int high)
{
	if (low < high) {
		int pos = partation(p, low, high);
		_Qsort(p, low, pos-1);
		_Qsort(p, pos+1, high);
	}	
}

void swap(int *p, int m, int n)
{
	int tmp;

	tmp = p[m];
	p[m] = p[n];
	p[n] = tmp;
}

int partation(int *p, int low, int high)
{
	int pv = p[low];
	
	while (low < high) {
		while (p[high] >= pv && low < high) {
			high --;	
		}
		swap(p, low, high);
		while (p[low] <= pv && low < high) {
			low ++;
		}
		swap(p, low, high);
	}
	
	return low;
}


