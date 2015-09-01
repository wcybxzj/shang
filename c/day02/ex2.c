#include <stdio.h>

int main(int argc, const char *argv[])
{
	int i = 0;
	int j = 0;
	int n = 10;
	int start_n = n - 1;
	for (i = 0; i < n; i++) {
		for (j = 0; j < i; j++) {
			printf(" ");
		}
		
		for (j = 0; j < 2*(start_n -i) + 1 ; j++) {
			printf("*");
		}

		printf("\n");
	}

	return 0;
}
