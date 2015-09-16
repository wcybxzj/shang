#ifndef SEQ_LIST_H
#define SEQ_LIST_H

//函数的声明、类型的定义

typedef void (*print_t)(const void *);
typedef int (*compare_t)(const void *, const void *);
typedef enum en{OK, FAIL}status;

typedef void DARR_T;

DARR_T *init_darr(int size, compare_t cmp);

status insert_darr_sort(DARR_T *, const void *);

status delete_darr(DARR_T *, const void *key, compare_t );

status search_darr(DARR_T *, const void *key, void *data, compare_t);

int set_sort(DARR_T *, compare_t);

void traval_darr(DARR_T *, print_t);

void destroy_darr(DARR_T *);

#endif
