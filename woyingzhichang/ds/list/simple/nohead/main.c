#include <stdio.h>
#include <stdlib.h>
#include "nohead.h"

int main(int argc, const char *argv[])
{
	NODE *list = NULL;
	STU tmp;
	int i;
	for (i = 0; i < 7; i++) {
		tmp.id = i;
		snprintf(tmp.name, NAMESIZE, "stu%d", i);
		tmp.math = rand() % 100;
		tmp.chinese = rand() % 100;
		list = list_insert(list, &tmp);
	}

	list_show(list);
	return 0;
}
