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
				if (get_top(tmp, &data) != 0) {
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

static int cal_num(char op, int left, int right)
{
	int res;

	switch (op) {
		case '+':
			res = left + right;break;
		case '-':
			res = left - right;break;
		case '*':
			res = left * right;break;
		case '/':
			res = left / right;break;
		default:
			exit(1);
	}	
	return res;
}

static int caculate(const char *ptr)
{
	STACK *res = NULL;
	int i;
	int tmp;
	char left, right;

	res = init_stack(sizeof(char));	
	if (NULL == res)
		exit(1);
	i = strlen(ptr)-1;	

	while (i >= 0) {
		if (isNumber(ptr[i])) {
			tmp = ptr[i] - '0';		
			push_stack(res, &tmp);
		} else if (isOperator(ptr[i])){
			if (!empty_stack(res)) {
				pop_stack(res, &left);
			} else 
				break;
			if (!empty_stack(res))
				pop_stack(res, &right);
			else 
				break;
			tmp = cal_num(ptr[i], left, right);
			push_stack(res, &tmp);
		}else
			break;
		i--;
	}

	if (i < 0 && !empty_stack(res)) {
		pop_stack(res, &tmp);	
		if (!empty_stack(res)) {
			exit(1);
		}
		return tmp;
	}
	exit(1);
}

int main(void)
{
	char buf[BUFSIZE] = {};

	if (transform("3*(2+1)-5+4", buf) < 0) {
		printf("expression is invalued\n");
	}else {
		puts(buf);
		printf("the result is %d\n", caculate(buf));
	}

	exit(0);
}
