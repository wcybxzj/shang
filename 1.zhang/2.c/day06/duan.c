#include <stdio.h>

int main(void)
{
	int var = 0x12345678;
	char *p = (char *)&var;

	(*p) == 0x78 ? printf("小端\n") : printf("大端\n");

	return 0;
}
