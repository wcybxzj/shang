#include <stdio.h>

int main(void)
{
	int i, j;
	int flag = 0;

	for (i = 100; i <= 1000; i++) {
		flag = 0;
		for (j = 2; j < i/2 + 1; j++) {
			if (i % j == 0) {
				flag = 1;
				break;	
			}
		}
		if (!flag) {
			printf("%d is a primer\n", i);
		}
	}

	return 0;
}
