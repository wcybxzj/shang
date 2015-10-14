#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	setuid(0);
	execlp("cat", "cat", "/etc/xinetd.conf", NULL);
	return 0;
}
