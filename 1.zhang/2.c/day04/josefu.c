#include <stdio.h>

int main(void)
{
	int arr[43] = {1};	
	int i;
	int cnt = 0;
	int kill = 0;

	for (i = 1; kill < 41 || !printf("game over\n"); i = (i+1)%43) {
		printf("is i:%d\n",i);
		if (!arr[i]) {
			cnt++;
			if (cnt == 3) {
				arr[i] = 1;
				printf("%d is killed\n", i);
				cnt = 0;
				kill ++;
			}
		}
	}

	for (i = 0; i < 43; i++) {
		if (arr[i] == 0) {
			printf("%d is alive\n", i);
			break;
		}
	}

	return 0;
}
