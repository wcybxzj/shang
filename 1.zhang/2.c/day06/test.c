#include <stdio.h>

void test(int *p)
{
	(*((char *)p + 1)) ++;
}

int main(void)
{
	int arr[] = {1,2,3,4};
	int i;

	test(arr);

	for (i = 0; i < 4; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
