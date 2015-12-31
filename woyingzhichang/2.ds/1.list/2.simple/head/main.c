#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(int argc, const char *argv[])
{
	int i, ret;
	int arr[] = {12, 9, -1, 23, 2, 34, 6, 45};
	list *ptr = NULL, *ptr2 = NULL; 
	ptr = list_create();
	ptr2 = list_create();
	if (NULL == ptr) {
		printf("error\n");
		exit(1);
	}

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		ret = list_insert_at(ptr, DATA_INIT_INDEX, &arr[i]);
		if (ret < 0) {
			fprintf(stderr, "isnert err %d\n", ret);
			exit(1);
		}
	}

	list_display(ptr);

	int tmp = 100;
	list_insert_at(ptr, 1, &tmp);
	list_display(ptr);
	list_destory(ptr);

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		ret = list_order_insert(ptr2, &arr[i]);
		if (ret < 0) {
			fprintf(stderr, "isnert err %d\n", ret);
			exit(1);
		}
	}

	list_display(ptr2);

	tmp = 23;
	list_delete(ptr2, &tmp);
	list_display(ptr2);

	list_delete_at(ptr2, 2, &tmp);
	printf("delete_at %d\n", tmp);
	list_display(ptr2);

	return 0;
}
