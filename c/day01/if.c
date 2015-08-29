#include <stdio.h>

int main(void)
{
	int var;

	printf("input a number:\n");
	scanf("%d", &var);	

	if (var % 2 == 0) {
		var++;
	}
	printf("var = %d\n", var);		

	return 0;
}
