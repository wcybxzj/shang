#include <stdio.h>
#include <string.h>
#include "linkstack.h"

typedef enum{NOT, NUM, OP, LEFT_OP} type_t;

type_t get_type(char s)
{
	if (s>='0' && s<='9') {
		return NUM;
	}
	if (s=='+' || s=='-' ||s=='*'|| s=='/' || s==')') {
		return OP;
	}
	if (s=='(') {
		return LEFT_OP;
	}
	return NOT;
}

void get_result(char *buf){
	STACK *result_stack = init_stack(sizeof(char));
	STACK *operator_stack = init_stack(sizeof(char));

	int i;
	int len1;
	char *str1 = "3*(2+1)-5/2*7";
	char tmp;
	type_t tmp_status;
	int is_move = 0;
	char left, right;

	len1 = strlen(str1);
	for (i = len1; i >= 0 ; i--) {
		tmp_status = get_type(str1[i]);
		if (tmp_status == NUM) {
			push_stack(result_stack, &str1[i]);
		}else if(tmp_status == OP){
			while(!empty_stack(operator_stack)) {
				is_move = 0;
				pop_stack(operator_stack, &tmp);
				if (str1[i]=='+' || str1[i]=='-') {
					if (tmp == '*'|| tmp=='/') {
						is_move=1;
						push_stack(result_stack, &tmp);
					}
				}
				if (is_move == 0) {
					push_stack(operator_stack,&tmp);
					break;
				}
			}
			push_stack(operator_stack, &str1[i]);
		}else if(tmp_status == LEFT_OP){
			while (!empty_stack(operator_stack)) {
				pop_stack(operator_stack, &tmp);
				if(tmp==')'){
					break;
				}else{
					push_stack(result_stack,&tmp);
				}
			}
		}
	}

	while (!empty_stack(operator_stack)) {
		pop_stack(operator_stack, &tmp);
		push_stack(result_stack, &tmp);
	}

	int j = 0;
	while(!empty_stack(result_stack)) {
		pop_stack(operator_stack, buf+j);
		j++;
	}

	destroy_stack(operator_stack);
	destroy_stack(operator_stack);
	return;
}


int main(int argc, const char *argv[])
{
	char buf[100]={};
	get_result(buf);
	printf("%s\n",buf);
	return 0;
}
