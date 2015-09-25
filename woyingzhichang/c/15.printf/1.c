#include <stdio.h>
#define SEC (1000*10000)
#define SEC2 (1000LL*10000LL)
int main(int argc, const char *argv[])
{
	printf("%d\n", SEC);
	printf("%lld\n", SEC2);
	return 0;
}
