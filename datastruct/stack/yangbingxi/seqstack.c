#include <stdlib.h>
#include <string.h>
#include "seqstack.h"


STACK *init_stack(int size){
	return init_darr(size);
}

status push_stack(STACK *ptr, const void *data){
	return insert_darr(ptr, data);
}

status pop_stack(STACK *ptr, void *data){
	int i;
	i = ptr->num-1;
	if (i>=0) {
		memcpy(data, ptr->arr+i*ptr->size, ptr->size);

		ptr->arr = realloc(ptr->arr, (ptr->num-1)*ptr->size);
		if (NULL == ptr->arr)
			return FAIL;

		(ptr->num)--;
	}
	return FAIL;
}

status empty_stack(STACK *ptr){
	return (ptr->num == 0);
}

void destroy_stack(STACK *ptr){
	destroy_darr(ptr);
}

