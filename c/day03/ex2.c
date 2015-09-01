#include <stdio.h>
int main(int argc, const char *argv[])
{
	int num = 0;
	do {
		scanf("%d", &num);
	} while ( (num > 100 ||num <60) && printf(" %s\n","again") );

	printf("ok\n");
	return 0;
}
