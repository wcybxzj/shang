#include <stdio.h>
#define M 3
#define N 4

void print_one_arr(int *p, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		printf("%4d", p[i]);
	}
}

//void print_two_arr(int (*p)[N], int m , int n){
void print_two_arr(int p[][N], int m , int n){
	printf("sizeof(p):%d\n", sizeof(p));//32 位　4
	int i, j;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			//printf("%4d", p[i][j]);
			printf("%4d ", *(*(p+i)+j));
		}
		printf("\n");
	}
}

void average_score(int *p, int num)
{
	int i = 0;
	float sum = 0;
	for (i = 0; i < num; i++) {
		sum += p[i];
	}
	printf("avg is %f\n", sum/num);
}

void find_num(int (*p)[N], int num){
	int i;
	for (i = 0; i < N; i++) {
		//printf("%d ", p[num][i]);
		printf("%d ", *(*(p+num)+i));
	}
}

int main(int argc, const char *argv[])
{
	int arr[M][N] = {1,2,3,4,5,6,7,8,9,10,11,12};

	printf("sizeof(a):%d\n", sizeof(arr));//48

	print_one_arr(&arr[0][0],M*N);//&a[0][0] = a[0] = *a = *(a+0)

	printf("\n");

	print_two_arr(arr, M, N);

	printf("\n");

	average_score(*arr, M*N);

	printf("\n");

	find_num(arr, 2);


	return 0;
}
