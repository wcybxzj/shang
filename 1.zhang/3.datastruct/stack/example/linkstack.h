#ifndef LINK_STACK_H
#define LINK_STACK_H

#include <llist.h>

typedef LLIST STACK;

STACK *init_stack(int size);

status_t push_stack(STACK *, const void *data);

status_t pop_stack(STACK *, void *data);

status_t get_top(STACK *, void *data);

status_t empty_stack(STACK *);

void destroy_stack(STACK *);

#endif
