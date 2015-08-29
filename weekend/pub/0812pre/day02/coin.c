#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	int var;

	var = rand() % 100;
	
	printf("var = %d\n", var);

	if (var >= 50) {
		printf("Sleep!!!....\n");
	} else {
		printf("Go to school....\n");
	}

	return 0;
}
