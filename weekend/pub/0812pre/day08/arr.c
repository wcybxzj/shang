#include <stdio.h>

int main(void)
{
	int arr[] = {1, 2};
	int i;

	printf("sizeof(i) = %d\n", sizeof(i));
	printf("sizeof(int) = %d\n", sizeof(int));
	printf("sizeof i = %d\n", sizeof i);

	for (i = 0; i < 2; i ++) {
		printf("%d\n", arr[i]);
		printf("%d\n", *(arr+i));
		printf("%d\n", *(i+arr));
		printf("%d\n", i[arr]);
	}
 
	return 0;
}
