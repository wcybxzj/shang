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

struct _HEAD{
	int size;
	NODE_ST head;
	int (*insert)(struct _HEAD *ptr, const void *data, int mode);
	void * (*find)(struct _HEAD *ptr, const void *key, cmp_t *cmp);
	int (*delete)(struct _HEAD *ptr, const void *key, cmp_t *func);
	int (*fetch)(struct _HEAD *ptr, const void *key, cmp_t *func, void *);
	void (*travel)(struct _HEAD *ptr, print_t pr);
	void (*destroy)(struct _HEAD* ptr);
};

struct _HEAD *llist_creat(int size);
int llist_insert(struct _HEAD *ptr, const void *data, int mode);
void * llist_find(struct _HEAD *ptr, const void *key, cmp_t *cmp);
int llist_delete(struct _HEAD *ptr, const void *key, cmp_t *func);
int llist_fetch(struct _HEAD *ptr, const void *key, cmp_t *func, void *);
void llist_travel(struct _HEAD *ptr, print_t pr);
void llist_destroy(struct _HEAD* ptr);


#endif
