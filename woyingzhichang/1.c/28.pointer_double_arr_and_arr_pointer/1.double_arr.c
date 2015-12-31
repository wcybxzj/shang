#include <stdio.h>
int main(int argc, const char *argv[])
{
	int a[2][3] = {1, 2, 3, 4, 5, 6};
	int i,j;

	printf("%p  %p\n", a, a+1);
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			//printf("%p -> %d\n", &a[i][j], a[i][j]);
			printf("%p -> %d\n", *(a+i)+j,*(*(a+i)+j));

		}
	}

	return 0;
}
