#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	int month;

	month = rand()%12+1;

#if 0
	if (month >= 3 && month <= 5) {
		printf("Spring...\n");
	} else if (month >= 6 && month <= 8) {
		printf("Summer...\n");
	} else if (month >= 9 && month <= 11) {
		printf("Automn...\n");
	} else {
		printf("Winter...\n");
	}
#endif
	switch (month) {
		case 12:
		case 1:
		case 2:
			printf("Winter...\n");
			break;	
		case 3:
		case 4:
		case 5:
			printf("Spring...\n");
			break;
		case 6:
		case 7:
		case 8:
			printf("Summer...\n");
			break;
		case 9:
		case 10:
		case 11:
			printf("Automn...\n");
			break;
		default:
			break;
	}

	return 0;
}
