#include <stdio.h>
#include <stdlib.h>

#include "linkstack.h"

static int isleft(char ch)
{
	return ch=='{'||ch=='['||ch=='('||ch=='<';
}

static int isright(char ch)
{
	return ch=='}'||ch==']'||ch==')'||ch=='>';
}

static int ismatch(char left, char right)
{
	int ret = 0;

	switch (left) {
		case '[':
			ret = (right == ']'); break;
		case '{':
			ret = (right == '}'); break;
		case '<':
			ret = (right == '>'); break;
		case '(':
			ret = (right == ')'); break;
		default :
			ret = 0; break;
	}	
	return ret;
}

static int scanner(const char *p)
{
	STACK *stack = init_stack(sizeof(char));	
	char tmp;
	int ret;
	if (stack == NULL)
		exit(1);

	while (*p) {
		if (isleft(*p)) {
			push_stack(stack, p);
		} else if (isright(*p)) {
			if (empty_stack(stack))
				break;
			pop_stack(stack, &tmp);	
		 	if (!ismatch(tmp, *p))
				break;
		} 
		p++;	
	}
	if (*p == '\0' && empty_stack(stack)) {
		destroy_stack(stack);
		return 0;
	}
	destroy_stack(stack);
	return -1;
}

int main(void)
{
	if (scanner("hel<lo(world) {good}[boy]>>") < 0) {
		printf("it 's doesn't match\n");
	} else {
		printf("matched!!\n");
	}
	
	exit(0);	
}
