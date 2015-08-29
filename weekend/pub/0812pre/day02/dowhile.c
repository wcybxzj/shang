#include <stdio.h>

int main(void)
{
	int grade;

	do {
		printf("input score:");
		scanf("%d", &grade);
	}while (grade < 0 || grade > 100);

	return 0;
}
