#include <stdio.h>

int main(void)
{
	int arr[3][4] = {
					{1,3,5,7},
					{11,33,55,77},
					{2,4,6,8}
	};
	int (*p)[4] = NULL;
	int * brr[3] = {arr[0], arr[1], arr[2]};

	p = arr;

	printf("arr = %p\n", arr);
	printf("arr[0] = %p\n", arr[0]);
	printf("arr[1] = %p\n", arr[1]);
	printf("\n");
	printf("arr +1 = %p\n", arr+1);
	printf("arr[0] +1 = %p\n", arr[0]+1);
	printf("arr[1] +1 = %p\n", arr[1]+1);

	printf("*************************\n");
	printf("arr[1] - arr[0] = %d\n", arr[1]-arr[0]);

	return 0;
}
