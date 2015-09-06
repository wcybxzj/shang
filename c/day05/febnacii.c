#include <stdio.h>

int febnaci(int n)
{
	if (n == 1 || n == 2) {
		return 1;
	} 
	return febnaci(n-1) + febnaci(n-2);	
}

int main(void)
{
	int i;

	for (i = 1; i <= 10; i++) {
		printf("%d ", febnaci(i));
	}
	printf("\n");

	return 0;
}
