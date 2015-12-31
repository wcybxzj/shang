#ifndef MY_LLIST_H
#define MY_LLIST_H

typedef enum{FAIL = -1, OK}status_t;
typedef enum{FRONT, REAR}way_t;

typedef void (*traval_t)(const void *);
typedef int (*compare_t)(const void *, const void *);

struct node_st {
	struct node_st *prev;
	struct node_st *next;
	char data[0];
};
typedef struct {
	struct node_st head;
	int size;
}LLIST;

LLIST *init_llist(int size);

status_t insert_llist(LLIST *ptr, const void *data, way_t way);

status_t delete_llist(LLIST *ptr, const void *key, compare_t cmp);

void *search_llist(LLIST *ptr, const void *key, compare_t cmp);

status_t fetch_llist(LLIST *ptr, const void *key, void *data, compare_t cmp);

int get_listnum(LLIST *ptr);

void reverse_llist(LLIST **ptr);

void reverse_r(LLIST *ptr);

void traval_llist(LLIST *ptr, traval_t op);

void destroy_llist(LLIST *ptr);

#endif
