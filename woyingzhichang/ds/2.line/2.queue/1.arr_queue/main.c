#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(int argc, const char *argv[])
{
	int i;
	Q *me;
	me = q_create();
	if (NULL == me) {
		return -1;
	}

	for (i = 0; i < MAXSIZE; i++) {
		if (q_enqueue(me, &i) == 0) {
			printf("enqueque ok %d\n", i);
		}else{
			printf("enqueque failed %d\n", i);
			break;
		}
	}
	q_travel(me);

	i = 0;
	while (1) {
		if (i==2) {
			break;
		}
		if (q_dequeue(me, &i) == 0) {
			printf("denquque ok %d\n", i);
		}else{
			printf("dequeued failed \n");
			break;
		}
		i++;
	}
	q_travel(me);

	while (1) {
		i  = rand() %500;
		if (q_enqueue(me, &i) == 0) {
			printf("enqueque ok %d\n", i);
		}else{
			printf("enqueque failed %d\n", i);
			break;
		}
	}
	q_travel(me);


	//
	q_clear(me);
	while (1) {
		i  = rand() %500;
		if (q_enqueue(me, &i) == 0) {
			printf("enqueque ok %d\n", i);
		}else{
			printf("enqueque failed %d\n", i);
			break;
		}
	}
	q_travel(me);

	q_destroy(me);

	return 0;
}
