#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	int *p =NULL;
	int num =5;
	int i;
	p = malloc(sizeof(int)*num);

	for (i = 0; i < num; i++) {
		scanf("%d", p+i);
		//p[i] = i;
	}

	for (i = 0; i < num; i++) {
		printf("%d ", p[i]);
	}

	return 0;
}
