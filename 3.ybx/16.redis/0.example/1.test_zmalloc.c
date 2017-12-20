#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"

void test_zmalloc();

int main(int argc, const char *argv[])
{
	test_zmalloc();
	return 0;
}

void test_zmalloc(){
	void *str = NULL;
	str = zmalloc(10);
	if (str==NULL) {
		perror("zmalloc()");
		exit(1);
	}
	strcpy(str,"abc");
	printf("%s\n",str);
}


