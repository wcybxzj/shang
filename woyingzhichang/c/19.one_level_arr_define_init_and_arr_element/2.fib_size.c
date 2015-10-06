#include <stdio.h>
#define SIZE 10

//sizeof避免把值写死
void fib()
{
	int arr[SIZE] = {1, 1};
	int i, j, tmp;

	for (i = 2; i < sizeof(arr)/sizeof(arr[0]); i++) {
		arr[i]  = arr[i-1] + arr[i-2];
	}
	printf("output:\n");
	for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
		printf("%d\n", arr[i]);
	}

	//reverse
	i = 0;
	j = sizeof(arr)/sizeof(arr[0])-1;

	while (i<j) {
		tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
		i++;j--;
	}
	printf("after reverse:\n");
	for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
		printf("%d\n", arr[i]);
	}
}


int main(int argc, const char *argv[])
{

	fib();

	return 0;
}
