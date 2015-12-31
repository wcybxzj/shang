#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	printf("Content-type: text/html\r\n\r\n");

	printf("<center><h1>%lld</h1></center>",(long long)time(NULL));


	exit(0);
}


