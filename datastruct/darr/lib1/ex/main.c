#include <stdio.h>
#include <seqlist.h>

#define NAMESIZE 32
struct stu_st{
	int id;
	char name[NAMESIZE];
	int grade;
};

void print_func(const void *p)
{
	const struct stu_st *p_data = p;
	printf("id:%d name:%s grade:%d\n", p_data->id, p_data->name, p_data->grade);
}

int id_compare(const void *key, const void *data)
{
	const int *k = key;
	const struct stu_st *d = data;
	return *k - d->id;
}

//gcc -I. main.c seqlist.c
int main(int argc, const char *argv[])
{
	DARR_T *you;
	int i, count;
	int id[] = {3, 2, 1, 9, 2, 4};
	struct stu_st tmp;

	you = init_darr(sizeof(struct stu_st));

	count = sizeof(id)/sizeof(*id);
	for (i = 0; i <count ; i++) {
		tmp.id = id[i];
		tmp.grade = 100 - id[i];
		snprintf(tmp.name, NAMESIZE, "dacong%d", id[i]);
		insert_darr(you, &tmp);
	}

	travel_darr(you, print_func);

	//id:3 name:dacong3 grade:97
	//id:2 name:dacong2 grade:98
	//id:1 name:dacong1 grade:99
	//id:9 name:dacong9 grade:91
	//id:2 name:dacong2 grade:98
	//id:4 name:dacong4 grade:96

	printf("删除后=============\n");//TODO
	int del_id = 9;
	delete_darr(you, &del_id, id_compare);
	travel_darr(you, print_func);


	return 0;
}
