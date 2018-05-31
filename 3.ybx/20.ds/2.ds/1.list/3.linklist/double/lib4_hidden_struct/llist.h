#ifndef LLIST_H__
#define LLIST_H__

#define LLIST_FORWARD  1
#define LLIST_BACKWARD 2

typedef void LLIST;

typedef void(*print_t)(const void *);
typedef int cmp_t(const void *,const void *);

LLIST *llist_creat(int size);
int llist_insert(LLIST *ptr, const void *data, int mode);
void * llist_find(LLIST *ptr, const void *key, cmp_t *cmp);
int llist_delete(LLIST *ptr, const void *key, cmp_t *func);
int llist_fetch(LLIST *ptr, const void *key, cmp_t *func, void *);
void llist_travel(LLIST *ptr, print_t pr);
void llist_destroy(LLIST* ptr);


#endif
