#include <stdio.h>

int main(void)
{
	signed int var1 = -20;
	unsigned int var2 = 6;

	if ((var1+var2) > 0) {
		printf("signed ----> unsigned\n");
	} else {
		printf("unsigned------>signed\n");
	}

	return 0;
}
