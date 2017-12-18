#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"

void test_adlist();

int main(int argc, const char *argv[])
{
	test_adlist();
	return 0;
}

void test_adlist()
{
	int arr[10]={11,22,33};
	list * mylist = listCreate();
	listAddNodeHead(mylist, &arr[0]);
	listAddNodeHead(mylist, &arr[1]);
	listAddNodeHead(mylist, &arr[2]);

	listIter * iter = listGetIterator(mylist, AL_START_HEAD);
	if (iter==NULL) {
		perror("listGetIterator() error");
		exit(1);
	}

	listNode *node;
	int *ptr;
	while((node = listNext(iter)) != NULL) {
		ptr = node->value;
		printf("%d\n",(*ptr));
	}
}
