#include <stdio.h>
#include <stdlib.h>
#include "proj.h"

static int i = 10;
int main(int argc, const char *argv[])
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
	call_func();
	exit(0);
}
