#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int ybx = 123;

//整形
void func(int **p)
{
	*p = malloc(sizeof(int));
	**p = 123;
}

//一维数组
void one_level_arr(int **p, int num)
{
	int i;
	srand(getpid());
	*p = malloc(sizeof(int) * num);
	if (*p == NULL) {
		printf("eeeeeeeeeeeeee\n");
	}
	for (i = 0; i < num; i++) {
		//*(*p+i) = rand() % 100;
		(*p)[i]= rand() % 100;
	}
}

//二维数组
void two_level_arr(int(**q)[5], int num)
{
	int i;
	int j;
	srand(getpid());
	*q = malloc(sizeof(int) * 5 * num);
	if (*q == NULL) {
		printf("eeeeeeeeeeeeee\n");
	}

	for (i = 0; i < num; i++) {
		for (j = 0; j < 5; j++) {
			//*(*(*q+i)+j) = rand() % 100;
			(*q)[i][j]= rand() % 100;
		}
	}
}

void output_func(int *q)
{
	printf("%d\n", *q);
}

void output_one_level(int *arr, int num)
{
	int i;
	for (i = 0; i < num; i++) {
		//printf("%d ", *(arr+i));
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void output_two_level_arr(int (*q)[5], int num)
{
	int i, j;
	for (i = 0; i < num; i++) {
		for (j = 0; j < 5; j++) {
			//printf("%d ", q[i][j]);
			printf("%d ", *(*(q+i)+j) );
		}
		printf("\n");
	}
}

void pointer_arr(int *arr[], int(*p)[5] , int row)
{
	int i, j;
	for (i = 0; i < row; i++) {
		//arr[i] = p[i];//&p[i][0]
		arr[i] = *(p+i);
		for (j = 0; j < 5; j++) {
			if (*arr[i] < p[i][j]) {
				arr[i] = &p[i][j];
			}
		}
	}
}


int main(int argc, const char *argv[])
{
	int i, j;
	printf("========== 普通指针 =============\n");
	int *p = NULL;
	func(&p);
	printf("%d\n", *p);
	output_func(p);

	printf("========== 一维数组 ==============\n");
	int *p1 = NULL;
	one_level_arr(&p1, 3);
	for (i = 0; i < 3; i++) {
		//printf("%d ", *(p1+i) );
		printf("%d ", p1[i] );
	}
	printf("\n");
	output_one_level(p1, 3);


	printf("========== 二维数组 ==============\n");
	int (*p2)[5] = NULL;
	two_level_arr(&p2, 3);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 5; j++) {
			//printf("%d ", *(*(p2 + i)+j));
			printf("%d ", p2[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	output_two_level_arr(p2, 3);

	printf("==========指针数组===============\n");
	int *arr[5] = {};
	pointer_arr(arr, p2, 3);
	for (i = 0; i < 3; i++) {
		printf("%d\n", *(arr[i]));
	}
	return 0;
}

