#include <stdio.h>

int main(void)
{
	int season;

	printf("input\n");
	scanf("%d", &season);

	if (season >= 3 && season <= 5)	 {
		printf("Spring\n");
	} else if (season > 5 && season < 9) {
		printf("Summer\n");
	} else if (season >= 9 && season <= 11) {
		printf("Autumn\n");
	} else {
		printf("Winter\n");	
	}

	return 0;
}
