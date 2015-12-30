#ifndef _SK_H__
#define _SK_H__

#define MAXSIZE 5
typedef int datatype;

typedef struct node_st{
	datatype data[MAXSIZE];
	int top;
} SK;

SK *st_create(void);

int st_isempty(SK *ptr);

int st_isfull(SK *ptr);

int st_push(SK *ptr, datatype *data);

int st_pop(SK *ptr, datatype *data);

//查看stack 顶部元素
int st_top(SK *ptr, datatype *data);

void st_travel(SK *ptr);

void st_destory(SK *ptr);

#endif
