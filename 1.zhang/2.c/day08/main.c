#include <stdio.h>

#define TEST

int main(void)
{	
#ifndef TEST
	printf("hello world\n");
#else
	printf("good morning\n");
#endif

	return 0;
}
