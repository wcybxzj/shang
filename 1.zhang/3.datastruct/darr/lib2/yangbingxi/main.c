#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include <seqlist.h>

#define NAMESIZE 32

struct stu_st {
	int id;
	char name[NAMESIZE];
	int score;
};

static int id_qsort(const void *a, const void *b){
	const struct stu_st *p_a = a;
	const struct stu_st *p_b = b;
	return p_a->id - p_b->id;
}

static int id_cmp(const void *key, const void *data)
{
	const struct stu_st *d = data;
	const int *k = key;

	return *k - d->id;
}

static int name_cmp(const void *key, const void *data)
{
	const struct stu_st *d = data;
	const char *k = key;

	return strcmp(k, d->name);
}

static void print_arr(const void *data)
{
	const struct stu_st *d = data;

	printf("%-4d%-10s%d\n", d->id, d->name, d->score);
}

int main(void)
{
	DARR_T *me;
	int id[] = {3,2,1,8,9,7};
	int id2[] ={0,4,11};
	int i;
	struct stu_st tmp;
	int num;
	
	me = init_darr(sizeof(struct stu_st));	
	if (me == NULL) {
		exit(1);
	}

	for (i = 0; i < sizeof(id) / sizeof(*id); i++) {
		tmp.id = id[i];
		snprintf(tmp.name, NAMESIZE, "bendan%d", id[i]);
		tmp.score = 100-id[i];
		insert_darr(me, &tmp);
	}

	traval_darr(me, print_arr);

	printf("\n\n");
#if 0
	num = 8;
	delete_darr(me, &num, id_cmp);
	delete_darr(me, "bendan1", name_cmp);
	traval_darr(me, print_arr);
#endif
	if (search_darr(me, "bendan7", &tmp, name_cmp) > 0) {
		printf("not found\n");	
	} else {
		printf("find it: %d %s\n", tmp.id, tmp.name);
	}


	destroy_darr(me);

	//homework
	printf("----------------homework qosrt-----------------\n");
	me = init_darr(sizeof(struct stu_st));
	for (i = 0; i < sizeof(id) / sizeof(*id); i++) {
		tmp.id = id[i];
		snprintf(tmp.name, NAMESIZE, "bendan%d", id[i]);
		tmp.score = 100-id[i];
		insert_darr(me, &tmp);
	}

	traval_darr(me, print_arr);
	printf("------------------------------------------------\n");
	qsort(me->arr, me->num, me->size, id_qsort);
	traval_darr(me, print_arr);

	printf("-----------------------顺序插入----------------\n");

	for (i = 0; i < sizeof(id2)/sizeof(*id2); i++) {
		tmp.id = id2[i];
		tmp.score = 100-id2[i];
		snprintf(tmp.name, NAMESIZE, "大葱%d", id2[i]);
		insert_darr_qsort(me, &tmp, &(tmp.id), id_qsort);
		traval_darr(me, print_arr);
	}


	return 0;
}
