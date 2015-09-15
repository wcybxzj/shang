#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <seqlist.h>

DARR_T *init_darr(int size){
	DARR_T *p = NULL;
	p = malloc(sizeof(*p));
	if (NULL == p) {
		return NULL;
	}
	p->arr = NULL;
	p->size = size;
	p->num = 0;
	return p;
}

status insert_darr(DARR_T *ptr, const void *data){
	void* tmp = NULL;//不知道需要操作什么类型的数据
	tmp = malloc( (ptr->num+1) * ptr->size );//分配比原来大1的空间
	if (NULL == tmp) {
		return FAIL;
	}
	memcpy(tmp, ptr->arr, ptr->num * ptr->size);
	memcpy(tmp+(ptr->num * ptr->size), data, ptr->size);
	(ptr->num)++;
	free(ptr->arr);
	ptr->arr = tmp;
	return OK;
}

status delete_darr(DARR_T *ptr, const void *key, compare compr){
	int i;
	for (i = 0; i < ptr->num; i++) {
		if (!compr(key, ptr->arr+ i * ptr->size)) {
			break;
		}
	}
	if (i == ptr->num) {
		return FAIL;
	}

	printf("delete i:%d\n", i);

	memmove(ptr->arr+(ptr->size*i), ptr->arr+(ptr->size*i+1),
			(ptr->num-i+1)*ptr->size);
	ptr->num--;
	struct stu_st *tmp;
	tmp = malloc(ptr->num * ptr->size);
	if (NULL == tmp) {
		return FAIL;
	}
	memcpy(tmp, ptr->arr, ptr->num * ptr->size);

	free(ptr->arr);
	ptr->arr = tmp;
	return OK;
}

void travel_darr(DARR_T *ptr, print_t pr){
	int i;
	for (i = 0; i < ptr->num; i++) {
		pr(ptr->arr + ptr->size*i);
	}
}
