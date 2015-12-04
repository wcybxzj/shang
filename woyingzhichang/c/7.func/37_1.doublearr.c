
#include <stdio.h>
#define M 3
#define N 4


//获取行指针
int * find_num(int (*p)[N], int num){
	if (num > M-1) {
		return NULL;
	}
	return *(p + num);
}

int main(int argc, const char *argv[])
{
	int i, j;
	int *tmp = NULL;
	//int arr[M][N] = {1,2,3,4,5,6,7,8,9,10,11,12};
	int arr[M][N] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};

	tmp = find_num(arr, 1);
	for (i = 0; i < N; i++) {
		printf("%d ", tmp[i]);
	}


	return 0;
}
