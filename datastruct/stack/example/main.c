#include <stdio.h>
#include <stdlib.h>

#include "linkstack.h"

int main(void)
{
	STACK *stack = NULL;
	int num, tmp, ret;

	stack = init_stack(sizeof(char));	
	if (NULL == stack)
		exit(EXIT_FAILURE);

	while (1) {
		printf("输入一个非负整数：(-1退出)\n");
		scanf("%d", &num);
		if (num == -1)
			break;
		if (num < 0) {
			printf("请重新输入\n");
			continue;
		}
	
		while (num) {
			tmp = num % 2;
			push_stack(stack, &tmp);
			num /= 2;	
		}

		while (!empty_stack(stack)) {
			pop_stack(stack, &tmp);
			printf("%d ", tmp);
		}
		printf("\n");
	}

	destroy_stack(stack);

	return 0;
}

