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
	if (search_darr(me, "bendan7", &tmp, name_cmp) < 0) {
		printf("not found\n");	
	} else {
		printf("find it: %d %s\n", tmp.id, tmp.name);
	}

	destroy_darr(me);

	return 0;
}
