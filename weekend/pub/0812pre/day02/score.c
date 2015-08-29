#include <stdio.h>

int main(void)
{
	int var;

	printf("please input your score:");
	scanf("%d", &var);

	switch (var / 10) {
		case 10:
		case 9:
			printf("A.....\n");
			break;
		case 8:
			printf("B.....\n");
			break;
		case 7:
			printf("C......\n");
			break;
		case 6:
			printf("D......\n");
			break;
		default:
			printf("work hard...\n");
			break;
	}

	return 0;
}
