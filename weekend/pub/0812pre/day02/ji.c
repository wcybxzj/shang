#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	int var;
	
	var = rand() % 1000;

	if (var % 2 == 0) {
		printf("偶数.....\n");
	} else {
		printf("奇数.....\n");
	}

	return 0;
}
