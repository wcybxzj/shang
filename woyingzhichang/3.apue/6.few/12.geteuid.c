#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

int main(int argc, const char *argv[])
{
	printf("ruid  %d, euid %d\n", getuid(), geteuid());
	return 0;
}
