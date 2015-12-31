#include <stdio.h>

int main(int argc, const char *argv[])
{
	printf("\33[2J");
	int i, j;
	for (i = 0; i < 2; i++) {
		printf("\33[100;100H ");
		for (j = 0; j < 5; j++) {
			printf("\33[43m ");
		}
		printf("\33[0m ");
		printf("\n");
	}
	return 0;
}
