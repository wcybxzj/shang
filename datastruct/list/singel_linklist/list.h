#ifndef MY_LIST_H
#define MY_LIST_H

typedef enum{OK, FAIL}status_t;
typedef enum{FRONT, REAR}way_t;

typedef void (*print_t)(const void *);
typedef int (*compare_t)(const void *, const void *);

struct node_st {
	void *data;
	struct node_st *next;
};
typedef struct {
	struct node_st head;
	int size;	
}LIST; 

LIST *init_head(int);

status_t insert_list(LIST *, const void *, way_t);

status_t delete_list(LIST *, const void *key, compare_t);

void traval_list(LIST *, print_t );

void destroy_list(LIST *);

#endif
