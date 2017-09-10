#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
4196040
4196045
56789012345678901234567890
*/
int main(){
	char *s = "0123456789012345678901234567890";
	char *p;
	p = strchr(s, '5');
	printf("%ld\n", s);
	printf("%ld\n", p);
	printf("%s\n",p);
	return 0;
}
