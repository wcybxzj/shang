#include <stdio.h>

void yang(int num)
{
	int arr[10][10] = {
		{1},
		{1, 1},
	};

	//arr[2][3] = 100;
	//printf("%d\n",arr[2][3]);

	int i, j = 0;
	for (i = 2; i < num; i++) {
		for (j = 0; j <= i ; j++) {
			if (j==0 || j==i) {
				arr[i][j] = 1;
			}else{	
				arr[i][j] = arr[i-1][j-1]+ arr[i-1][j];
			}
		}
		printf("\n");
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}

}

int main(int argc, const char *argv[])
{
	yang(10);
	return 0;
}
