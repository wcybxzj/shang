#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(int argc, const char *argv[])
{
	int i, ret;
	int arr[] = {11,22,33,4};
	list *ptr = NULL; 
	ptr = list_create();
	if (NULL == ptr) {
		printf("error\n");
		exit(1);
	}

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		ret = list_insert(ptr, 1, &arr[i]);
		printf("ret is %d\n", ret);
	}

	list_display(ptr);

	return 0;
}
