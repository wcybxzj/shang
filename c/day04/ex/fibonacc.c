#include <stdio.h>
int main(int argc, const char *argv[])
{
	//1 1 2 3 4
	int i;
	int arr[20] = {1, 1};
	for (i = 2; i < 20 || !printf("\n=========\n"); i++) {
		arr[i] = arr[i-1] + arr[i-2];
		printf("%d ", arr[i]);
	}
	return 0;
}
