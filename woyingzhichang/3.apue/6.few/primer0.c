#include <stdio.h>
#include <stdlib.h>

#define LEFT  30000000
#define RIGHT 30000200

int main(int argc, const char *argv[])
{
	int i, j, mark;
	for (i = LEFT; i <= RIGHT ; i++) {
		mark = 1;
		for (j = 2; j < i/2 ; j++) {
			if (i % j == 0) {
				mark = 0;
				break;
			}
		}
		if (mark) {
			printf("%d is primer\n", i);
		}
	}
	return 0;
}
