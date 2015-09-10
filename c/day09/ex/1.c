#include <stdio.h>

void func(int *arr)
{	int i;
	for (i = 0; i < 3; i++) {
		printf("%d\n", arr[i]);
	}
}

int main(int argc, const char *argv[])
{
	int arr[]= {11,22,33,44,55};
	func(&arr[2]);

	printf("%d\n", 3/2);

	return 0;
}
