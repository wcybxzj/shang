#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

int main(void)
{
	QUEUE *me = NULL;
	int count;
	int arr[] = {1,2,3,4,5,6,7,8,9,10};
	int i, j, tmp;

	me = init_queue();
	if (me == NULL) {
		fprintf(stderr, "init fail\n");
		exit(1);
	}	

	count =  sizeof(arr)/sizeof(*arr);
	printf("count is %d\n", count);

	for (j = 0; j < 5; j++) {
		for (i = 0; i < count; i++) {
			if (FAIL == enq_queue(me, arr[i])) {
				printf("enq faild arr[i] is %d \n", arr[i]);
				break;
			}
		}
		for (i = 0; i < 3; i++) {
			if (FAIL == deq_queue(me, &tmp)) {
				printf("deq fail\n");
				break;
			}
			printf("%d ", tmp);
		}
		printf("\n");
	}

	destroy_queue(me);

	return 0;
}
