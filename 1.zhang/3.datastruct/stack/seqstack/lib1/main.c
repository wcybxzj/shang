#include <stdio.h>
#include <stdlib.h>

#include "seqstack.h"

int main(void)
{
	datatype_t arr[] = {1,2,3,4,5,6,7,8,9};
	int i, ret;
	STACK *me = NULL;

	me = initstack();
	if (me == NULL)
		exit(EXIT_FAILURE);

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
