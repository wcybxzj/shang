#include <stdio.h>

//报错
//#include "2.h"

//办法:
extern int add( int x, int y );

int main(int argc, const char *argv[])
{
	int a;
	a = add(2,3);
	printf("%d\n",a);
	return 0;
}
