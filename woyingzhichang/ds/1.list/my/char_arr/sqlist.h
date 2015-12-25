#ifndef SQLIST_H__
#define SQLIST_H__

#define DATASIZE 1024

typedef char* datatype;	

typedef struct node_st{
	datatype data[DATASIZE];
	int last;//last+1 为插入的位置
}sqlist;

sqlist *sqlist_create();

void sqlist_create1(sqlist **);

//第3个参数传递指针避免复制引起更大的开销
int sqlist_insert(sqlist *, int i, datatype *);

int sqlist_delete(sqlist *, int i);

int sqlist_find(sqlist *, datatype *);

int sqlist_isempty(sqlist *);

int sqlist_setempty(sqlist *);

int sqlist_getnum(sqlist *);

void sqlist_display(sqlist *);

int sqlist_destory(sqlist *);

int sqlist_union(sqlist *, sqlist *);

#endif
