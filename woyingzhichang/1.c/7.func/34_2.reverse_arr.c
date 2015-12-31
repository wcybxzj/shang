#include <stdio.h>

void func(int p[], int n)
{
	int tmp;
	int *end_p = NULL;
	int i;
	for (i = 0; i < n; i++) {
		end_p = p+i;
	}

	while (p < end_p) {
		tmp = *p;
		*p = *end_p;
		*end_p = tmp;
		p++;
		end_p--;
	}

}


int main(int argc, const char *argv[])
{
	int i;
	int arr[] = {1, 3, 4, 5, 7, 9};
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		printf("%d\n", arr[i]);
	}

	func(arr, sizeof(arr)/sizeof(*arr));

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		printf("%d\n", arr[i]);
	}

	return 0;
}
