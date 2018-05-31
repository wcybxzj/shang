#include <stdio.h>
#include <stdlib.h>
#include "sqstack.h"

SK *st_create(void){
	SK* st= NULL;
	st = malloc(sizeof(SK));
	if (NULL == st) {
		return NULL;
	}
	st->top = -1;
	return st;
}

int st_isempty(SK *st){
	return (st->top==-1);
}

int st_isfull(SK *st){
	return (st->top == MAXSIZE -1);
}

int st_push(SK *st, datatype *data){
	if (st_isfull(st)) {
		return -1;
	}
	st->data[++st->top] = *data;
	return 0;
}

int st_pop(SK *st, datatype *data){
	if (st_isempty(st))
		return -1;

	*data = st->data[st->top--];

	return 0;
}

//查看stack 顶部元素
int st_top(SK *st, datatype *data){
	if (st_isempty(st))
		return -1;

	*data = st->data[st->top];
	return 0;
}

void st_travel(SK *st){
	int i;
	if (st_isempty(st))
		return;

	for (i = 0; i <= st->top; i++) {
		printf("%d ", st->data[i]);
	}
	printf("\n");
}

void st_destory(SK *st){
	free(st);
}
