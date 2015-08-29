#include <stdio.h>

int main(void)
{	
	int i;

	i = -10;
	while (1) {
		if (i == 0) {
			i++;//i = i+1;
			continue;
		}
		printf("10 / %3d = %3d\n", i, 10/i);
		i++;
		if (i == 10) {
			break;
		}
	}

	return 0;
}
