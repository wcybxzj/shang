#include <stdio.h>

#if 0
*
* *
* * *
* * * *

#endif

int main(void) 
{
	int i, j;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < i+1; j++) {
			printf("* ");
		}
		printf("\n");
	}

	return 0;
}
