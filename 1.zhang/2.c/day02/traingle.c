#include <stdio.h>

int main(void)
{
	int row;
	int i, j;

	row = 10;
	for (i = 0; i < row; i ++) {
		for (j = 0; j < i; j++) {
			printf("  ");
		}
		for (j = 0; j < 2*row-1-2*i; j++) {
			printf("* ");	
		}
		printf("\n");
	}


	return 0;
}
