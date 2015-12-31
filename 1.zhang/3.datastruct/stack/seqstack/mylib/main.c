#include <stdio.h>
#include <stdlib.h>

#include "seqstack.h"

#define NAMESIZE 32

typedef struct {
	int id;
	char name[NAMESIZE];
}STU;

int main(void)
{
	int i;
	int arr[] = {11,22,33,44,55,66,77,88,99};
	STU * stu_tmp;
	STACK *me = NULL;
	me = initstack(sizeof(STU));
	stu_tmp = malloc(sizeof(STU));//TODO

	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {


		pushstack(me, arr[i]);	
	}
	while (1) {
		ret = popstack(me, &i);
		if (ret == FAIL)
			break;
		printf("%d ", i);
	}
	printf("\n");

	destroystack(me);

	return 0;
}
