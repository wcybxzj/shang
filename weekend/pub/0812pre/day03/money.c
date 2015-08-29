#include <stdio.h>

int main(void)
{
	int i, j;
	int cnt = 0;

	for (i = 0; i <= 10; i++) {
		for (j = 0; j <= 20; j++) {
			if ((100-i*10-j*5 >= 0) && (100-i*10-j*5) % 2 == 0) {
				cnt ++;
			}
		}
	}
	printf("cnt = %d\n", cnt);

	return 0;
}
