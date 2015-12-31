#include <stdlib.h>
#include <string.h>

#include "seqstack.h"

STACK *initstack(void)
{
	STACK *stack = NULL;

	stack = malloc(sizeof(*stack));
	if (NULL == stack)
		return NULL;
	memset(stack, 0x00, sizeof(*stack));	
	stack->top = 0;

	return stack;
}

status emptystack(STACK *stack)
{
	return stack->top == 0;
}

status fullstack(STACK *stack)
{
	return stack->top == STACKSIZE;
}

status pushstack(STACK *stack, datatype_t  data)
{
	if (fullstack(stack))
		return ERROR;
	stack->arr[stack->top++] = data;

	return RIGHT;
}

status popstack(STACK *stack, datatype_t *data)
{
	if (emptystack(stack))
		return ERROR;
	*data = stack->arr[--stack->top];

	return RIGHT;
}
int get_stacknum(STACK *stack)
{
	return stack->top;
}

void destroystack(STACK *stack)
{
	free(stack);	
}

