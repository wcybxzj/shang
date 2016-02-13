#include <stdio.h>
#include <unistd.h>
int main(int argc, const char *argv[])
{
	alarm(5);
	while (1);
	return 0;
}
