#include <stdio.h>

int main(void)
{
	char ch = 'a';
	short sh = 4;

	printf("*****%d******\n", sizeof(ch+sh));

	return 0;
}
