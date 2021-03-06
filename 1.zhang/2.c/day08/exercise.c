#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_arr(int (**p)[5], int row);
void find_max(int **arr, int (*p)[5], int row);
void find_max2(int *max_arr, int (*p)[5], int row);
void output(int (*p)[5], int row);


int main(void)
{
	int (*p)[5] = NULL;
	int *arr[4] = {};//都初始化为0
	int max_arr[4]= {};
	int row, i;

	row=4;

	create_arr(&p, row);

	output(p, row);

	find_max(arr, p, row);
	printf("\n");
	for (i = 0; i < 4; i++) {
		printf("%d ", *arr[i]);
	}

	find_max2(max_arr, p, row);
	printf("\n");
	for (i = 0; i < row; i++) {
		printf("%d ", max_arr[i]);
	}
	printf("\n");

	free(p);

	return 0;
}


void output(int (*p)[5], int row)
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < 5; j++) {
			printf("%3d ", p[i][j]);
		}
		printf("\n");
	}
}


int create_arr(int (**q)[5], int row)
{
	srand(getpid());
	int i, j;	

	*q = malloc(sizeof(int) * 5 * row);
	if (NULL == *q) {
		return -1;
	}
	memset(*q, 0x00, sizeof(int) * 5 * row);
	for (i = 0; i < row; i++) {
		for (j = 0; j < 5; j++) {
			(*q)[i][j] = rand() % 50;	
		}
	}

	return 0;
}

void find_max(int **arr, int (*p)[5], int row)
{
	int i, j;

	for (i = 0; i < row; i++) {
		//数组名等于数组首元素的地址
		//p[i]就是数组名等于&p[i][0]
		arr[i]  = p[i];
		for (j = 0; j < 5; j++) {
			if (*arr[i] < p[i][j]) {
				arr[i] = &p[i][j];
			}	
		}
	}	
}

void find_max2(int *max_arr, int (*p)[5], int row){
	int i, j;
	for (i = 0; i < row; i++) {
		 max_arr[i] = p[i][0];
		 for (j = 1; j < 5; j++) {
			 if (max_arr[i] < p[i][j]) {
				 max_arr[i] = p[i][j];
			 }
		 }
	}
}
