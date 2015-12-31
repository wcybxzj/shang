#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p;

	p = malloc(sizeof(*p));
	/*if error*/
	
	*p = 1;
	
	printf("%p->%d\n",p,*p);

	free(p);
	p = NULL;

	*p = 10;
	printf("%p->%d\n",p,*p);


	exit(0);
}


