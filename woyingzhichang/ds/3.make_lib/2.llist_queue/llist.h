#ifndef LLIST_H__
#define LLIST_H__

#define LLIST_FORWARD  1
#define LLIST_BACKWARD 2

typedef void(*print_t)(const void *);
typedef int cmp_t(const void *,const void *);

typedef struct _NODE {
	struct _NODE* prev;
	struct _NODE* next;
	char data[1];
} NODE_ST;

typedef struct _LLIST{
	int size;
	NODE_ST head;
} LLIST;

LLIST *llist_create(int size);
int llist_insert(LLIST *ptr, const void *data, int mode);
void * llist_find(LLIST *ptr, const void *key, cmp_t *cmp);
int llist_delete(LLIST *ptr, const void *key, cmp_t *func);
int llist_fetch(LLIST *ptr, const void *key, cmp_t *func, void *);
void llist_destroy(LLIST* ptr);


#endif
