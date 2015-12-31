#include <stdio.h>

void func(int arr[])
{
	int i;
	for (i = 0; i < 3; i++) {
		printf("%d\n", arr[i]);
	}
}

int main(int argc, const char *argv[])
{
	int arr[3]= {11,22,33};
	func(arr);
	return 0;
}
