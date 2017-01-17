#include <stdio.h>
int main(int argc, const char *argv[])
{
#ifdef __GNUC__
	printf("complier is GNU C complier gcc\n");
#endif
	return 0;
}
