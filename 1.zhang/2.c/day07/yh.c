#include <stdio.h>

#define ROW 10

void yanghui(int (*p)[ROW], int m)
{
	int i, j;

	for (i = 0; i < m; i++) {
		p[i][0] = 1;
		for (j = 1; j <= i; j++) {
			if (i >= 1) {
				p[i][j] = p[i-1][j] + p[i-1][j-1];
			}
		}
	}	
	
}

int main(void)
{
	int arr[ROW][ROW] = {};	
	int i, j;

	yanghui(arr, ROW);

	for (i = 0; i < ROW; i++) {
		for (j = 0; j <= i; j ++) {
			printf("%-4d", arr[i][j]);	
		}
		printf("\n");
	}

	return 0;
}

