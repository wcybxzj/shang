#include <stdio.h>
#include "binheap.h"
#define MAXSIZE (10)

int main(int argc, const char *argv[])
{
	PriorityQueue H;

	H = Init(MAXSIZE);

	int i,j;
	for (i = 0,j = MAXSIZE/2; i < MAXSIZE; i++, j = (j+71)%MAXSIZE) {
		printf("要插入的值是:%d\n", j);
		Insert(j, H);
	}
	for (i = 0; i <= H->size; i++) {
		printf("%d\n",H->arr[i]);
	}
	int tmp;
	while (!IsEmpty(H)) {
		tmp = DeleteMin(H);
		printf("delte:%d\n", tmp);
	}

	return 0;
}
