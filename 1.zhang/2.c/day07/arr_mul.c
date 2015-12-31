#include <stdio.h>

int main(void)
{
	int i, j;
	int arr[3][3] = {
		{1,2,3},
		{4,5,6},
		{7,8,9}
	};	
	int (*p)[3] = arr;

	//p++;//&arr[1]
	//printf("%d\n", *p+5);
	//*p -> arr[1]
	// arr[1]+5
	// &arr[1][5]
	// 最后结果：9的地址

	printf("sizeof(arr) = %d\n", sizeof(arr));

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}

	//arr = 0x7fff89ff5b10
	//arr+1 = 0x7fff89ff5b1c
	//&arr = 0x7fff89ff5b10
	//&arr+1 = 0x7fff89ff5b34
	printf("arr = %p\n", arr);
	printf("arr+1 = %p\n", arr+1);
	printf("&arr = %p\n", &arr);
	printf("&arr+1 = %p\n", &arr+1);

	printf("===============\n");
	printf("%d\n", p[2][2]);//9
	printf("%d\n", (*(p+2))[2]);//9

	return 0;
}
