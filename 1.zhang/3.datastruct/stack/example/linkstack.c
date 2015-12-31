#include <stdlib.h>
#include "linkstack.h"

STACK *init_stack(int size)
{
	return init_llist(size);
}

status_t push_stack(STACK *mystack, const void *data)
{
	return insert_llist(mystack, data, FRONT);	
}

static int always(const void *d1, const void *d2)
{
	return 0;
}

status_t pop_stack(STACK *mystack, void *data)
{
	return fetch_llist(mystack, NULL, data, always);
}

status_t empty_stack(STACK *mystack)
{
	return empty_llist(mystack);
}

status_t get_top(STACK *mystack, void *data)
{
	return get_firstnode(mystack, data);
}

void destroy_stack(STACK *mystack)
{
	destroy_llist(mystack);
}

