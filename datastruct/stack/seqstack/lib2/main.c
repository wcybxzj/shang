#include <stdio.h>
#include <stdlib.h>

#include "seqstack.h"

int main(void)
{
	struct stack_st *mystack = NULL;	
	int id[] = {3,2,1,6,7,9,4};
	int i, ret;

	mystack = init_seqstack();
	if (NULL == mystack)
		exit(1);
	
	for (i = 0; i < sizeof(id)/sizeof(*id); i++) {
		push_seqstack(mystack, id[i]);
	}
	while (1) {
		ret = pop_seqstack(mystack, &i);
		if (ret == FAIL)
			break;
		printf("%d ", i);
	}
	printf("\n");

	destroy_seqstack(mystack);
	
	exit(0);
}
