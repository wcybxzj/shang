#include <stdlib.h>
#include <string.h>

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

status insert_darr(DARR_T *ptr, const void * data){
	void* tmp = NULL;//不知道需要操作什么类型的数据
	tmp = malloc((ptr->num+1) *ptr->size);//分配比原来大1的空间
	if (NULL == tmp) {
		return FAIL:
	}


}
