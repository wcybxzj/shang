#include <stdio.h>

struct {
	unsigned int ur:1;
	unsigned int uw:1;
	unsigned int ux:1;
	unsigned int gr:1;
	unsigned int gw:1;
	unsigned int gx:1;
	unsigned int or:1;
	unsigned int ow:1;
	unsigned int ox:1;
}mode;

int main(void)
{
	mode.ur = 1, mode.uw = 1, mode.ux = 0;
	mode.gr = 1, mode.gw = 0, mode.gx = 0;
	mode.or = 0, mode.ow = 1, mode.ox = 0;

	printf("mode: %d%d%d %d%d%d %d%d%d\n", mode.ur, mode.uw,\
			mode.ux, mode.gr, mode.gw, mode.gx, mode.or, 
			mode.ow, mode.ox);

	printf("sizeof(mode) = %d\n", sizeof(mode));

	return 0;
}
