#include<stdio.h>
#include<stdlib.h>


__attribute ((visibility("default"))) void not_hidden ()
{
	printf("exported symbol/n");
}

void is_hidden ()
{
	printf("hidden one/n");
}
