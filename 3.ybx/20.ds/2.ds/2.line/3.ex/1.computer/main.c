#include <stdio.h>
#include <stdio.h>
#include "sqstack.h"

int get_pri(int op)
{
	switch(op){
		case '(':
			return 0;
		case '+':
		case '-':
			return 1;
		case '*':
		case '/':
			return 2;
		default:
			return 0;
	}
}

static void compute(SK *s_num,datatype *op){
	datatype v1, v2, v3;
	st_pop(s_num, &v1);
	st_pop(s_num, &v2);
	switch(*op){
		case '+':
			v3 = v1+v2;
			break;
		case '-':
			v3 = v2-v1;
			break;
		case '*':
			v3 = v2*v1;
			break;
		case '/':
			v3 = v2/v1;
			break;
		default:
			break;
	}
	st_push(s_num, &v3);
}

static void deal_bracket(SK *s_op, SK *s_num)
{
 	datatype old_op;
	st_top(s_op, &old_op);
	while (old_op != '(') {
		st_pop(s_op, &old_op);
		compute(s_num, &old_op);
		st_top(s_op, &old_op);
	}
	st_pop(s_op, &old_op);// pop ')'
}

static void deal_op(SK *s_op, SK *s_num, int op)
{
	datatype old_op;
	if (st_isempty(s_op) || op == '(') {
		st_push(s_op, &op);
		return;
	}
	st_top(s_op, &old_op);
	if (get_pri(op) > get_pri(old_op)) {
		st_push(s_op, &op);
		return;
	}
	while (get_pri(op) <= get_pri(old_op)) {
		st_pop(s_op, &old_op);
		compute(s_num, &old_op);
		if (st_isempty(s_op)) {
			break;
		}
		st_top(s_op, &old_op);
	}
	st_push(s_op, &op);
}

int main(int argc, const char *argv[])
{
	int i = 0;
	char *str = "10+(11+3)*2-5";
	char item;
	SK *st_op = NULL, *st_num = NULL;
	int val = 0;
	datatype old_op;

	st_op = st_create();
	st_num = st_create();

	while (str[i] != '\0') {
		if (isdigit(str[i])) {
			val = val * 10 + (str[i]-'0');
		}else{//is a op
			if (val) {
 				st_push(st_num, &val);
	 			val = 0;
			}
			if (str[i] == ')') {//右括号弹栈计算直到找到左括号
				deal_bracket(st_op, st_num);
			}else{//+ - * / (
				deal_op(st_op, st_num, str[i]);
			}
		}
		i++;
	}
	if (val) {
		st_push(st_num, &val);
	}

	while (!st_isempty(st_op)) {
		st_pop(st_op, &old_op);
		compute(st_num, &old_op);
	}

	st_pop(st_num, &val);
	printf("%d\n", val);

	st_destory(st_num);
	st_destory(st_op);

	return 0;
}
