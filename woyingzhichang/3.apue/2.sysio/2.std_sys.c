#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	putchar('a');
	write(1,"b",1);
	putchar('a');
	write(1,"b",1);
	return 0;
}
