#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkstack.h"
#include "linkqueue.h"

#define BUFSIZE 32

static int isNumber(char ch)
{
	return ch >= '0' && ch <= '9';
}

static int isOperator(char ch)
{
	return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

static int isLeft(char ch)
{
	return ch == '(';
}

static int isRight(char ch)
{
	return ch == ')';
}

static int priority(char ch)
{
	int ret = 0;
	if (ch == '+' || ch == '-') 
		ret = 1;
	if (ch == '*' || ch == '/')
		ret = 2;
	return ret;	
}

static int transform(const char *ptr, char *buf){

	QUEUE *op_stack = NULL;
	STACK *result_queue = NULL;
	const char *end  = NULL;
	const char *start = ptr;
	char stack_top, stack_tmp;

	op_stack= init_stack(sizeof(char));
	if (op_stack== NULL)
		exit(1);
	result_queue = init_stack(sizeof(char));
	if (result_queue == NULL)
		exit(1);
	
	end = ptr+strlen(ptr)-1;
	while (start <= end) {
		if (isNumber(*start)) {
			enq_queue(op_stack, start);
		}else if (isOperator(*start)) {
			while (1) {
				if (get_top(op_stack, &stack_top) != 0) {
					push_stack(tmp, start);
					break;
				} else {
					if ( priority(*start) == priority(stack_top) ||
							priority(*start) < priority(stack_top) ) {
						pop_stack(op_stack, &stacK_tmp);//弹栈
						enq_queue(result_queue, &stacK_tmp);//入队列
					} else {
						push_stack(tmp, start);
						break;
					}
				}
			}
		} else if (isLeft(*end)) {
			push_stack(tmp, end);	
		} else if (isRight(*end)) {
			while (1) {
				pop_stack(tmp, &num);
				if (num == ')')
					break;
				push_stack(res, &num);
			}	
		} else {
			break;
		}

		start++;
	}

}

int main(void)
{
	char buf[BUFSIZE] = {};

	if (transform("3*(2+1)-5+4", buf) < 0) {
		printf("expression is invalued\n");
	}else {
		puts(buf);
		//printf("the result is %d\n", caculate(buf));
	}

	exit(0);
}
