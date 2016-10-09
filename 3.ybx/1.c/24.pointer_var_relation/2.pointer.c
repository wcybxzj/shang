#include <stdio.h>
int main(int argc, const char *argv[])
{
	double i =123.4567;
	double *p = &i;
	char *w = &i;
	printf("%f\n", *p);
	printf("%f\n", *w);
	return 0;
}
