#include <stdio.h>

int main(void)
{
	int var;

	while (1) {
		printf("input a number:");
		scanf("%d", &var);
		if (var % 2 == 0) {
			break;
		}
	}
	printf("get a even\n");

	return 0;
}
