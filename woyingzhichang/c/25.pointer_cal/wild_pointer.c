#include <stdio.h>
int main(int argc, const char *argv[])
{
	int *p;
	*p =123;//段错误
	return 0;
}
