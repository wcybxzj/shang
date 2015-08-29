#include <stdio.h>
#if 0
	*		  3    0   3
   * *		       1   1
  *   *			   2   0
 *     *      4    0   5   2*n-3-(2*i+1)
  *   * 	       1   3
   * *			   2   1
    *		       3   0
-----------------
	*
   * *
  *   *
 *     *    
*		*
 *     *       5    0  7   
  *   *				1  3
   * *				2  1
    *
#endif

int main(void)
{
	int row;
	int i, j;	

	printf("how many rows:");
	scanf("%d", &row);

	for (i = 0; i <= row/2; i++) {
		for (j = 0; j < row/2-i; j++) {
			printf(" ");
		}
		printf("*");
		if (i == 0) {
			printf("\n");
			continue;
		}
		for (j = 0; j < 2*i-1; j++) {
			printf(" ");
		}
		printf("*");
		printf("\n");
	}
	//下半部分
	for (i = 0; i < row / 2; i++) {
		for (j = 0; j <= i; j++) {
			printf(" ");
		}
		printf("*");
		if (i == row/2-1) {
			printf("\n");
			break;
		}
		for (j = 0; j < row-3-(2*i+1); j++) {
			printf(" ");
		}
		printf("*");
		printf("\n");
	}

	return 0;
}
