#ifndef LIST_H__
#define LIST_H__

typedef int  datatype;
typedef struct node_st{
	datatype data;
	struct node_st *next;
}list;

list *list_create();

int list_insert(list *, int i, datatype *);
int list_order_insert(list *, datatype *);
void list_display(list *);
int list_delete_at(list *, int i, datatype *);//删除返回删除节点
int list_delete(list *, datatype *);
int list_isempty(list *);
void list_destory(list *);
#endif
