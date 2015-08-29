#include <stdio.h>

int main(void)
{
	char arr[5] = "abcd";
	
	char *const p = arr;

	*(p + 1) = 'e';
//	p = p + 1;

	printf("%s\n", arr);

	return 0;
}
