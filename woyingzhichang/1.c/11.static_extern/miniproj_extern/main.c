#include <stdio.h>
#include <stdlib.h>
#include "proj.h"

int i = 10;
int main(int argc, const char *argv[])
{
	printf("[%s] i:%d\n", __FUNCTION__, i);
	func();
	printf("[%s] i:%d\n", __FUNCTION__, i);
	exit(0);
}
