#include <stdio.h>
#include <strings.h>
#include <string.h>

#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	char buf[BUFSIZE];
	bzero(buf, BUFSIZE);
	buf[0]='a';
	strcat(buf, "b");
	printf("%s\n",buf);
	return 0;
}
