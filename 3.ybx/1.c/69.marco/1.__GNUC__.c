#include <stdio.h>
int main(int argc, const char *argv[])
{
#ifdef __GNUC__
	printf("complier is GNU C complier gcc\n");
	printf("__GNUC__:%d\n",__GNUC__);
#endif
	return 0;
}
