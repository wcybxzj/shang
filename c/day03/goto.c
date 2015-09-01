#include <stdio.h>

int main(void)
{
	int i;

	for (i = 0; i < 100; i++) {
		if (i == 50) {
			goto error;
		}
	}	

	return 0;
error:
	printf("i = %d\n", i);
	return -1;
}
