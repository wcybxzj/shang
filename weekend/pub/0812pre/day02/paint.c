#include <stdio.h>

int main(void)
{
	int i, j;
	int n;

	printf("how many:");
	scanf("%d", &n);

	for (i = 0; i < n; i++) {
		for (j = 0; j < n-i-1; j++) {
			printf(" ");
		}
		for (j = 0; j < 2*i+1; j++) {
			printf("*");
		}
		printf("\n");
	}
	printf("\n\n");
	for (i = 0; i < n; i++) {
		for (j = 0; j < i; j++) {
			printf(" ");
		}
		for (j = 0; j < 2*n-1-2*i; j++) {
			printf("*");
		}
		printf("\n");
	}

	return 0;
}
