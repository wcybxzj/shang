#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int *p = NULL;

	p = malloc(sizeof(int));
	if (NULL == p)
		return 1;
	memset(p, 0x00, sizeof(int));
	
	*p = 6;

	free(p);
	p = NULL;

	return 0;
}
