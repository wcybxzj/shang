#include <stdio.h>

int main(void)
{
	int var;
	int i;
	int flag = 0;
	int cnt = 0;

	while (1) {
		do {
			printf("input a number:");
			scanf("%d", &var);
		}while (var < 0);

		flag = 0;
		for (i = 2; i < var; i++) {
			if (var % i == 0) {
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			cnt ++;
			if (cnt == 10) {
				break;
			}
			printf("%d is primer\n", var);
		} else {
			printf("%d isn't primer\n", var);
		}
	}
	printf("you got %d primer\n", cnt);

	return 0;
}
