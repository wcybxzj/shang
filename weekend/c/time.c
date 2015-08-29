#include <stdio.h>
#include <stdlib.h>

int main()
{
	
	printf("Content-type: text/html\r\n\r\n");

	printf("<center><h1>It's now:%lld</h1></center>",(long long)time(NULL));


	exit(0);
}


