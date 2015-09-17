#include <stdlib.h>
#include <string.h>

#include "seqstack.h"

struct stack_st *init_seqstack(void)
{
	struct stack_st *me = NULL;

	me = malloc(sizeof(*me));
	if (me == NULL)
		return NULL;
	me->base = malloc(sizeof(datatype_t) * BASESIZE);
	if (me->base == NULL)
		return NULL;
	me->top = me->base;
	me->stacksize = BASESIZE;
	
	return me;
}

status_t empty_seqstack(struct stack_st *stack)
{
	return stack->top == stack->base;
	//return me->top-me->base == 0;
}

status_t full_seqstack(struct stack_st *stack)
{
	return stack->top - stack->base == stack->stacksize;	
}

status_t push_seqstack(struct stack_st *stack, datatype_t data)
{
	if (full_seqstack(stack)) {
		stack->base = realloc(stack->base, (stack->stacksize + INCREACE) * sizeof(datatype_t));
		if (stack->base == NULL) {
			return FAIL;
		}
		stack->top = stack->base + stack->stacksize;
		*stack->top++ = data;
		stack->stacksize += INCREACE;
	}else {	
		*stack->top++ = data;
	}

	return OK;
}

status_t pop_seqstack(struct stack_st *stack, datatype_t *data)
{
	if (empty_seqstack(stack)) 
		return FAIL;
	stack->top --;
	*data = *stack->top;

	return OK;
}

void destroy_seqstack(struct stack_st *stack)
{
	free(stack->base);
	free(stack);
}

