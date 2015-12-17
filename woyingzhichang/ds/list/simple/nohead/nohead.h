#ifndef NOHEAD_H__
#define NOHEAD_H__ 

//无头不循环单链表
#define NAMESIZE 32
typedef struct score_st {
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
}STU;

typedef struct node_st{
	STU data;
	struct node_st *next;
}NODE;

NODE *list_insert(NODE *ptr, STU *data);
void list_show(NODE *ptr);
int list_delete(NODE *ptr);
NODE *list_find(NODE *ptr, NODE *data);

#endif

