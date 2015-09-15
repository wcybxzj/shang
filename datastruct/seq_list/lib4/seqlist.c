#include <stdlib.h>
#include <string.h>

#include <seqlist.h>

struct seq_st{
	char *arr;
	int size;
	int num;	
	compare_t cmp;
};

DARR_T *init_darr(int size, compare_t cmp)
{
	struct seq_st *p = NULL;

	p = malloc(sizeof(*p));
	if (NULL == p)	
		return NULL;
	p->arr = NULL;
	p->size = size;
	p->num = 0;
	p->cmp = cmp;

	return p;
}

static int bsearch_near(struct seq_st *ptr, const void *data)
{
	int start, end, mid;
	int ret;

	start = 0; 
	end = ptr->num-1;
	
	while (start <= end) {
		mid = (start+end) / 2;
		ret = ptr->cmp(data, ptr->arr+mid*ptr->size);
		if (ret == 0) 
			break;
		if (ret > 0) {
			start = mid + 1;
		} else {
			end = mid - 1;
		}
	}	
	return mid;
}

status insert_darr_sort(DARR_T *p, const void *data)
{
	struct seq_st *ptr = p;
	int ind;
	ptr->arr = realloc(ptr->arr, (ptr->num+1)*ptr->size);
	if (NULL == ptr->arr)
		return FAIL;
	
	if (ptr->num == 0 || ptr->cmp == NULL) {
		ind = 0;
	} else {
		ind = bsearch_near(ptr, data);
		if (ptr->cmp(data, ptr->arr+ind*ptr->size) > 0) {
			ind++;
		}
	}
	memmove(ptr->arr+(ind+1)*ptr->size, ptr->arr+ind*ptr->size, \
			(ptr->num-ind)*ptr->size);
	memcpy(ptr->arr+ind*ptr->size, data, ptr->size);

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

