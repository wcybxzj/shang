#include <stdio.h>
#include <stdio.h>
#include "sqstack.h"

int main(int argc, const char *argv[])
{
	int i;
	datatype arr[] = {11, 22, 33, 44, 55, 66, 77};
	SK *st = NULL;
	st = st_create();
	if (NULL == st) {
		return -1;
	}
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		if (st_push(st, &arr[i]) == 0 ) {
			printf("push ok :%d\n", arr[i]);
		}else{
			printf("push fail :%d\n", arr[i]);
			break;
		}
	}
	st_travel(st);

	datatype tmp;
	while (st_pop(st, &tmp)==0) {
		printf("pop: %d\n", tmp);
	}

	st_destory(st);
	return 0;
}
