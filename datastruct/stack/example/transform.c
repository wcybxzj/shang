#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkstack.h"

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

static int transform(const char *ptr, char *buf)
{
	STACK *res = NULL;		
	STACK *tmp = NULL;		
	const char *end = NULL;
	char data, num;
	int i;

	res = init_stack(sizeof(char));
	if (res == NULL)
		exit(1);
	tmp = init_stack(sizeof(char));
	if (tmp == NULL)
		exit(1);

	end = ptr+strlen(ptr)-1;
	while (end >= ptr) {
		if (isNumber(*end)) {
			push_stack(res, end);	
		} else if (isOperator(*end)) {
			while (1) {
				if (get_top(tmp, &data) == FAIL) {
					push_stack(tmp, end);
					break;
				}else {
					if (priority(*end) < priority(data)) {
						pop_stack(tmp, &num);
						push_stack(res, &num);	
					} else {
						push_stack(tmp, end);
						break;
					}		
				}
			}	
		} else if (isRight(*end)) {
			push_stack(tmp, end);	
		} else if (isLeft(*end)) {
			while (1) {
				pop_stack(tmp, &num);
				if (num == ')')
					break;
				push_stack(res, &num);
			}	
		} else {
			break;
		}
		end--;
	}
	if (end >= ptr) {
		destroy_stack(tmp);
		destroy_stack(res);
		return -1;
	}
	
	while (!empty_stack(tmp)) {
		pop_stack(tmp, &num);
		push_stack(res, &num);	
	}
	i = 0;
	while (!empty_stack(res)) {
		pop_stack(res, buf+i);
		i++;	
	}

	destroy_stack(tmp);
	destroy_stack(res);

	return 0;
}

int main(void)
{
	char buf[BUFSIZE] = {};
	if (transform("3*(2+1)-5/2*7", buf) < 0) {
		printf("expression is invalued\n");
	}else {
		puts(buf);
	}

	exit(0);
}
