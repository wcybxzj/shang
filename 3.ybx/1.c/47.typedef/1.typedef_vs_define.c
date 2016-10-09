#include <stdio.h>
#include <stdlib.h>
/*
情况1:效果相同
#define INT int
typedef int INT;

INT i;-->int i;


情况2:效果相同
#define IP int*
typedef	int *IP;

IP p;--> int *;


情况3:效果不同
IP p, q;--> int *p, q;
IP p, q;--> int *p, *q;

*/
int main(int argc, const char *argv[])

	return 0;
}
