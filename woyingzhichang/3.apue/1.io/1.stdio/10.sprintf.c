#include <stdio.h>

int main(int argc, const char *argv[])
{
	char buf[1024];
	int y =1024, m = 5, d =12;
	snprintf(buf, "%d-%d-%d",y,m,d);
	printf("%s\n", buf);
	return 0;
}
