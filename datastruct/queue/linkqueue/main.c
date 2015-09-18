#include <stdio.h>
#include <stdlib.h>

#include "linkqueue.h"

int main(void)
{
	int arr[] = {5,7,1,6,8,9,2,3,4};
	QUEUE *my;
	int i;

	my = init_queue(sizeof(int));	
	if (NULL == my) {
		exit(1);
	}
	
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		enq_queue(my, arr+i);
	}
	
	while (!empty_queue(my)) {
		deq_queue(my, &i);
		printf("%d ", i);
	}
	printf("\n");

	destroy_queue(my);

	exit(0);
}
