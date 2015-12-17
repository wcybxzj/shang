#ifndef LIST_H__
#define LIST_H__
//有头单链表非循环链表

//数据节点当做第0个节点
#define DATA_INIT_INDEX 0
#define TRUE 1
#define FALSE 0



typedef int  datatype;

typedef struct node_st{
	datatype data;
	struct node_st *next;
}list;

list *list_create();

int list_insert_at(list *, int i, datatype *);
int my_list_insert_at(list *, int i, datatype *);
int list_order_insert(list *, datatype *);
void list_display(list *);
int list_delete_at(list *, int i, datatype *);//删除返回删除节点
int list_delete(list *, datatype *);
int list_isempty(list *);
void list_destory(list *);
#endif
