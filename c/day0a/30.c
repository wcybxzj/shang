#include <stdio.h>
int main(int argc, const char *argv[])
{
	int w[3][3] ={11,22,33,44,55,66,77,88,99};
	int (*pw)[3];
	pw = w;

	printf("%d\n",*(pw+1)[1]);

	return 0;
}
