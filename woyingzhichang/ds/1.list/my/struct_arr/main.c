#include <stdio.h>
#include <stdlib.h>
#include "sqlist.h"

int main(int argc, const char *argv[])
{
	int i, err;
	sqlist *list = NULL;
	sqlist *list2 = NULL;
	datatype arr[] = {
		{1,"abc"}, {2,"def"}, {3,"hig"}, {4,"klm"}, {5,"nop"}
	};
	datatype arr2[] = {
		{1,"qrw"}, {2,"xyz"}, {3,"abc"}, {4,"klm"}, {5,"nop"}
	};

	//list = sqlist_create();
	sqlist_create1(&list);
	sqlist_create1(&list2);
	if (NULL == list) {
		fprintf(stderr, "sqlist_create failed!\n");
		exit(1);
	}

	//
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		if ( (err = sqlist_insert(list, 0, &arr[i])) != 0) {
			if (err==-1) {
				fprintf(stderr, "arr is full!\n");
			} else if(err == -2) {
				fprintf(stderr, "pos is wrong\n");
			} else {
				fprintf(stderr, "Error\n");
			}
			exit(1);
		}
	}
	sqlist_display(list);

	//
	sqlist_delete(list, 1);
	sqlist_display(list);

	//
	datatype find = {
		.id = 2,
		.name = "def"
	};
	i = sqlist_find(list, &find);
	printf("%d\n", i);

	//
	for (i = 0; i < sizeof(arr2)/sizeof(*arr2); i++) {
		if ( (err = sqlist_insert(list2, 0, &arr2[i])) != 0) {
			if (err==-1) {
				fprintf(stderr, "arr2 is full!\n");
			} else if(err == -2) {
				fprintf(stderr, "pos is wrong\n");
			} else {
				fprintf(stderr, "Error\n");
			}
			exit(1);
		}
	}
	sqlist_display(list2);

	//
	printf("union:\n");
	sqlist_display(list);
	sqlist_display(list2);
	sqlist_union(list, list2);
	sqlist_display(list);

	sqlist_destory(list);
	sqlist_destory(list2);

	exit(0);
}
