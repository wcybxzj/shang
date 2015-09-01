#include <stdio.h>

int main(void)
{
	int i, j;
	
	for (j = 0; j < 10; j++) {
		for (i = 0; i < 10; i ++) {
			printf("*");
		}
		printf("\n");
	}

	return 0;
}
