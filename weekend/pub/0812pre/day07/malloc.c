#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *p, *q;

	p = malloc(20);

	q = p;
	scanf("%s %s", p, q);
	printf("%s %s\n", p, q);

	return 0;
}
