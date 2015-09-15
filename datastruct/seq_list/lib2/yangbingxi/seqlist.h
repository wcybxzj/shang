#ifndef SEQ_LIST_H
#define SEQ_LIST_H

//函数的声明、类型的定义

typedef void (*print_t)(const void *);
typedef int (*compare_t)(const void *, const void *);
typedef enum en{OK, FAIL}status;

typedef struct seq_st{
	char *arr;
	int size;
	int num;	
}DARR_T;

DARR_T *init_darr(int size);

status insert_darr(DARR_T *, const void *);

status insert_darr_qsort(DARR_T *ptr, const void *data, const void *key, compare_t cmp);

status delete_darr(DARR_T *, const void *key, compare_t );

status search_darr(DARR_T *, const void *key, void *data, compare_t);

void traval_darr(DARR_T *, print_t);

void destroy_darr(DARR_T *);

#endif
