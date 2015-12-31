#include <stdio.h>

int main(void)
{
	int i, j;
	int cnt = 0;
	//2块 5块 10块
	for (i = 0; i <= 50; i++) {
		for (j = 0; j <= 20; j++) {
			if ((100-i*2 - j*5) >= 0 && (100-2*i-5*j)%10 == 0) {
				cnt ++;
			}
		}
	}
	printf("cnt = %d\n", cnt);

	return 0;
}
