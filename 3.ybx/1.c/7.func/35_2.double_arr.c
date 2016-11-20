#include <stdio.h>

#define M 3
#define N 4

void func1(int *arr)
{
	arr[1] = 222222;
}

void one_level_test()
{
	int i;
	int arr[5] = {11,22,33,44,55};//5x4=20
	func1(arr);
	for (i = 0; i < 5; i++) {
		printf("%d ", arr[i]);
	}
}

func2(int arr[][N]){
	int i, j;
	for (i = 0; i < M; i++) {
		//var++;
		for (j = 0; j < M; j++) {
			//var++;
			if (i==0 && j ==2) {
				arr[i][j] = 445566;
			}
		}
	}
}

void two_level_test()
{
	int i, j;
	int arr[M][N] = {
		{11,22,33,44},
		{55,66,77,88},
		{99,100,101,102}
	};

	func2(arr);
	for (i = 0; i < M; i++) {
		for (j = 0; j < N; j++) {
			printf("%d ",arr[i][j]);
		}
	}
}

int main(int argc, const char *argv[])
{
	//one_level_test();
	two_level_test();
	return 0;
}
