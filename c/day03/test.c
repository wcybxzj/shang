#include <stdio.h>

int main(void)
{
	for (int i = 0 ; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			printf("\33[43m  ");
		}
		printf("\n");
	}
	printf("\33[0m");
	return 0;
}
