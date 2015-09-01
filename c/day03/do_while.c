#include <stdio.h>

int main(void)
{
	int grade;	

	do {
		printf("input your grade:");
		scanf("%d", &grade);	
		if (grade < 0 || grade > 100) {
			grade = 0;
			printf("invalued, input again\n");
			continue;
		}
		
	} while (grade < 60);

	printf("congratulations! pass it\n");

	return 0;
}
