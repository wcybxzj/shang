#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main(void)
{
	QUEUE *me = NULL;
	int arr[] = {1,2,3,4,5,6,7,8,9,10};
	int i, j, tmp;

	me = init_queue();
	if (me == NULL) {
		fprintf(stderr, "init fail\n");
		exit(1);
	}	

	for (j = 0; j < 5; j++) {
		for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
			enq_queue(me, arr[i]);
		}
		for (i = 0; i < 3; i++) {
			deq_queue(me, &tmp);
			printf("%d ", tmp);
		}
		printf("\n");
	}

	destroy_queue(me);

	return 0;
}
