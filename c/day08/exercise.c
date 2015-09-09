#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_arr(int (**p)[5], int row);
int find_max(int **arr, int (*p)[5], int row);

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

int main(void)
{
	int (*p)[5] = NULL;
	int *arr[4] = {};
	int row, i;

	printf("input rows\n");
	scanf("%d", &row);

	create_arr(&p, row);

	output(p, row);

	find_max(arr, p, row);

	printf("\n");
	for (i = 0; i < 4; i++) {
		printf("%d ", *arr[i]);
	}
	printf("\n");

	free(p);

	return 0;
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

int find_max(int **arr, int (*p)[5], int row)
{
	int i, j;

	for (i = 0; i < row; i++) {
		arr[i]  = p[i];
		for (j = 0; j < 5; j++) {
			if (*arr[i] < p[i][j]) {
				arr[i] = &p[i][j];
			}	
		}
	}	
}

