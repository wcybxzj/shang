#include <stdlib.h>
#include <string.h>

#include <seqlist.h>

struct seq_st{
	char *arr;
	int size;
	int num;	
};

DARR_T *init_darr(int size)
{
	struct seq_st *p = NULL;

	p = malloc(sizeof(*p));
	if (NULL == p)	
		return NULL;
	p->arr = NULL;
	p->size = size;
	p->num = 0;

	return p;
}

status insert_darr(DARR_T *p, const void *data)
{
	struct seq_st *ptr = p;
	ptr->arr = realloc(ptr->arr, (ptr->num+1)*ptr->size);
	if (NULL == ptr->arr)
		return FAIL;
	memcpy(ptr->arr+ptr->num*ptr->size, data, ptr->size);

	(ptr->num) ++;

	return OK;
}

status delete_darr(DARR_T *p, const void *key, compare_t cmp)
{
	int i;		
	char *tmp = NULL;
	struct seq_st *ptr = p;

	for (i = 0; i < ptr->num; i++) {
		if (!cmp(key, ptr->arr+i*ptr->size)) {
			break;
		}
	}
	if (i == ptr->num)
		return FAIL;

	memmove(ptr->arr+i*ptr->size, ptr->arr+(i+1)*ptr->size, \
			(ptr->num-(i+1))*ptr->size);
	(ptr->num) --;
	ptr->arr = realloc(ptr->arr, ptr->num * ptr->size);
	if (NULL == ptr->arr)
		return FAIL;

	return OK;
}

status search_darr(DARR_T *p, const void *key, void *data, compare_t cmp)
{
	struct seq_st *ptr = p;
	int i;

	for (i = 0; i < ptr->num; i++) {
		if (!cmp(key, ptr->arr + i*ptr->size)) {
			memcpy(data, ptr->arr + i*ptr->size, ptr->size);
			return OK;
		}
	}	

	return FAIL;
}

int set_sort(DARR_T *p, compare_t cmp)
{
	struct seq_st *ptr = p;

	qsort(ptr->arr, ptr->num, ptr->size, cmp);

	return OK;
}

void traval_darr(DARR_T *p, print_t op)
{
	int i;
	struct seq_st *ptr = p;

	for (i = 0; i < ptr->num; i++) {
		op(ptr->arr+i*ptr->size);
	}
}

void destroy_darr(DARR_T *p)
{
	struct seq_st *ptr = p;
	free(ptr->arr);	
	free(ptr);
}

