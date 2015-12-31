#include <stdio.h>

#define	LEFT 	 30000000
#define RIGHT	 30000200

int main(void)
{
	int i, j;	
	int cnt = 0;
	int flag;

	for (i = LEFT; i <= RIGHT; i++) {
		flag = 0;
		for (j = 2; j < i/2+1; j++) {
			if (i % j == 0) {
				flag = 1;
				break;
			}
		}	
		if (!flag) {
			printf("%d is a primer\n", j);
			cnt ++;
		}
	}
	printf("cnt = %d\n", cnt);

	return 0;
}
