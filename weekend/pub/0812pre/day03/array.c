#include <stdio.h>

int main(void)
{
	int test[5] = {1,2,3,4,5};
	int i;
   	
	printf("%d\n", test[0]);
	printf("%d\n", test[1]);
	printf("%d\n", test[2]);
	printf("%d\n", test[3]);
	printf("%d\n", test[4]);

	for (i = 0; i < 5; i++) {
		test[i] = i+10;
	}
	for (i = 0; i < 5; i++) {
		printf("%d ", test[i]);
	}
	printf("\n");
	return 0;
}
