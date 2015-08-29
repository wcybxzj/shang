#include <stdio.h>

int main(void)
{
	int arr[] = {1,6,9,4};
	int *p = arr;
	int var;
	
	var = *(p++);
	printf("var = %d\n", var);
	
	return 0;
}
