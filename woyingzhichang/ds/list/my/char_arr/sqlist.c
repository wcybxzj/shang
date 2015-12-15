#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlist.h"

sqlist *sqlist_create(){
	sqlist *me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}
	me->last = -1;
	return me;
}

void sqlist_create1(sqlist **ptr){
	*ptr = malloc(sizeof(**ptr));
	if (NULL == *ptr) {
		return;
	}
	(*ptr)->last = -1;
}

int sqlist_insert(sqlist *me, int i, datatype *data){
	if (me->last == DATASIZE-1) {
		return -1;
	}

	//last+1为现在应该顺序插入的位置
	if (i < 0 || i > me->last+1) {
		return -2;
	}

	int j = me->last;
	for (; i <= j; j-- ) {
		me->data[j+1] = me->data[j];
	}
	me->data[i] = *data;//存在缺陷//TODO
	me->last++;
	return 0;
}

int sqlist_delete(sqlist *me, int i){
	if (i < 0 || i > me->last) {
		return -1;
	}
	int j = me->last;
	for (; i < j; i++) {
		me->data[i] = me->data[i+1];
	}
	me->last--;
	return 0;
}

int int_compare(int num1, int num2){
	if (num1 == num2) {
		return 1;
	}
	return 0;
}

int str_compare(char *str1, char *str2){
	if (strcmp(str1, str2) == 0) {
		return 1;
	}
	return 0;
}

int sqlist_find(sqlist *me, datatype *data){
	if (sqlist_isempty(me)) {
		return -1;
	}
	int i;
	for (i = 0; i <= me->last; i++) {
		//if (me->data[i] == *data) {
		if (str_compare(me->data[i], *data)) {
			return i;
		}
	}
	return -2;
}

int sqlist_isempty(sqlist *me){
	if (me->last == -1) {//empty
		return 1;
	}
	return 0;
}

int sqlist_setempty(sqlist *me){
	me->last = -1;
	return 0;
}

int sqlist_getnum(sqlist *me){
	return me->last+1;
}

void sqlist_display(sqlist *me){
	if (me->last == -1) {
		return;
	}
	int i;
	for (i = 0; i <= me->last; i++) {
		printf("%s ",me->data[i]);
	}
	printf("\n");
}

int sqlist_destory(sqlist *me){
	free(me);
	return 0;
}

int sqlist_union(sqlist *list1, sqlist *list2){
	int i, num;
	for (i = 0; i <= list2->last; i++) {
		if ( (num = sqlist_find(list1, &list2->data[i])) == -2) {
			sqlist_insert(list1, 0, &list2->data[i]);
		}
	}
	return 0;
}
