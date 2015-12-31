#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int create_arr(int (**p)[4][5], int row);
void output(int (*p)[4][5], int row);

int main(int argc, const char *argv[])
{
	int (*p)[4][5] = NULL;
	create_arr(&p, 3);
	output(p, 3);
	return 0;
}

void output(int (*p)[4][5], int row)
{
	int i, j, k;
	for (i = 0; i < row; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 5; k++) {
				printf("%3d ", p[i][j][k]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

int create_arr(int (**q)[4][5], int row)
{
	srand(getpid());
	int i, j, k;	

	*q = malloc(sizeof(int) * 4 * 5 * row);
	if (NULL == *q) {
		return -1;
	}

	memset(*q, 0x00, sizeof(int) * 4 * 5 * row);
	for (i = 0; i < row; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 5; k++) {
				(*q)[i][j][k] = rand() % 50;	
			}
		}
	}

	return 0;
}

