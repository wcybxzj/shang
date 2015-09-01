#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i = 0;
	for (i = 0; i < 3; i++) {
		printf("\33[50;50H ");
		printf("\33[43m ");
		printf("\33[43m ");
		printf("\33[43m ");
		printf("\33[43m ");
		printf("\33[43m ");
		printf("\33[43m ");
		printf("\33[0m ");
		printf("\n");
	}
	return 0;
}
