#ifndef SEQ_LIST_H
#define SEQ_LIST_H

typedef void (*print_t)(const void*); //print_t 是类型
typedef int(* compare)(const void *, const void*);
typedef enum en{OK, FAIL} status;

typedef struct seq_t{
	void *arr; //万能指针来指向数据
	int num;//数据个数
	int size;//数据类型
}DARR_T;

//初始化只需要size	
DARR_T *init_darr(int size);

//插入任何类型的数据 
status insert_darr(DARR_T *ptr, const void *data);

status delete_darr(DARR_T *ptr, const void *key, compare compr);

status search_darr(DARR_T *ptr, const void *key, const void *data, compare compr);

//打印
void travel_darr(DARR_T *ptr, print_t pr);

void destory(DARR_T *ptr);



#endif
