#include <stdio.h>

#define	LEFT 	 30000000
#define RIGHT	 30000200

int main(void)
{
	int i, j;	
	int cnt = 0;

	for (i = LEFT; i <= RIGHT; i++) {
		for (j = 2; j < i/2+1; j++) {
			if (i % j == 0) {
				break;
			}
		}	
		if (j == i/2+1) {
			printf("%d is a primer\n", j);
			cnt ++;
		}
	}
	printf("cnt = %d\n", cnt);

	return 0;
}
