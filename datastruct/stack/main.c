#include <stdio.h>
#include <stdlib.h>

#include "linkstack.h"

struct data_st {
	int id;
	int grade;
};

int main(void)
{
	STACK *stack;
	struct data_st tmp;
	int idarr[] = {1,2,3,4,5,6,7,8,9};
	int i;

	stack = init_stack(sizeof(struct data_st));	
	if (stack == NULL) 
		exit(1);
	for (i = 0; i < sizeof(idarr)/sizeof(*idarr); i++) {
		tmp.id = idarr[i];
		tmp.grade = 100 - idarr[i];		
		push_stack(stack, &tmp);
	}

	while (!empty_stack(stack)) {
		pop_stack(stack, &tmp);	
		printf("%-3d%-3d\n", tmp.id, tmp.grade);
	}

	destroy_stack(stack);

	return 0;
}

