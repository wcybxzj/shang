#include <stdlib.h>
#include <string.h>

#include "seqstack.h"

typedef struct stack_st {
	int top;
	char arr[0];
}STACK;


STACK *initstack(int size)
{
	STACK *stack = NULL;

	stack = malloc(sizeof(*stack) + size*STACKSIZE);
	if (NULL == stack)
		return NULL;
	memset(stack, 0x00, sizeof(*stack));	
	stack->top = 0;

	return stack;
}

status_t emptystack(STACK *stack)
{
	return stack->top == 0;
}

status_t fullstack(STACK *stack)
{
	return stack->top == STACKSIZE;
}

status_t pushstack(STACK *stack, datatype_t  data)
{
	if (fullstack(stack))
		return FAIL;
	stack->arr[stack->top++] = data;

	return OK;
}

status_t popstack(STACK *stack, datatype_t *data)
{
	if (emptystack(stack))
		return FAIL;
	*data = stack->arr[--stack->top];

	return OK;
}

void destroystack(STACK *stack)
{
	free(stack);	
}

