#include <stdio.h>

int main(void)
{
	int arr[3][4] = {
		{1,2,3,4},
		{5,6,7,8},
		{9,10,11,12}
	};		

	printf("arr     = %p\n", arr);
	printf("&arr[0] = %p\n", &arr[0]);

	printf("arr + 1 = %p\n", arr+1);
	printf("&arr    = %p\n", &arr);
	printf("&arr +1 = %p\n", &arr+1);

	printf("arr[2]+3 = %p\n", arr[2]+3);

	return 0;
}
