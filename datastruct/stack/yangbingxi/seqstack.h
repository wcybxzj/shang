#ifndef SQELIST_H
#define SQELIST_H

#include <seqlist.h>

typedef DARR_T STACK;

STACK *init_stack(int size);

status push_stack(STACK *, const void *data);

status pop_stack(STACK *, void *data);

status empty_stack(STACK *);

void destroy_stack(STACK *);

#endif

