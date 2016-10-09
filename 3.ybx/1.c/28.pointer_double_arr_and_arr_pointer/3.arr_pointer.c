#include <stdio.h>
//数组指针
int main(int argc, const char *argv[])
{
	int a[2][3] = {1, 2, 3, 4, 5, 6};
	int i,j;
	int (*q)[3];
	q = a;

	printf("%p  %p\n", a, a+1);
	printf("%p  %p\n", q, q+1);

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			//printf("%p -> %d\n", *(a+i)+j,*(*(a+i)+j));
			printf("%p -> %d\n", *(q+i)+j,*(*(q+i)+j));
		}
	}

	return 0;
}
