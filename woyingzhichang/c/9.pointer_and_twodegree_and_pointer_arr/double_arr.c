#include <stdio.h>
int main(int argc, const char *argv[])
{
	int arr[2][3] = {11,22,33,44,55,66};
	int *p;

	p = arr;
	int i = 0;
	for (i = 0; i < 6; i++) {
		printf("%d\n",p[i]);
	}


	return 0;
}
