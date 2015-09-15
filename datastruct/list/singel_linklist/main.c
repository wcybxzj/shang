#include <stdio.h>
#include <stdlib.h>

#include <list.h>

#define NAMESIZE 32

struct data_st {
	char name[NAMESIZE];	
	int age;
	int sex;
};

static void print_fun(const void *data)
{
	const struct data_st *d = data;

	printf("%-5s%-4d%c\n", d->name, d->age, d->sex);
}

static int age_cmp(const void *key, const void *data)
{
	const struct data_st *d = data;
	const int *k = key;
	return *k - d->age;
}	

int main(void)
{
	srand(time(NULL));
	LIST *my;
	struct data_st tmp;
	int i;

	my = init_head(sizeof(struct data_st));
	if (NULL == my) 
		exit(1);
	
	for (i = 0; i < 10; i++) {
		snprintf(tmp.name, NAMESIZE, "%c%c%c",'a'+rand()%26, 'a'+rand()%26, rand()%26+'a');	
		tmp.age = 20 + i;
		tmp.sex = rand()%2==0?'f':'m';

		insert_list(my, &tmp, REAR);
	}
	traval_list(my, print_fun);
	printf("\n\n");

	int age = 25;
	delete_list(my, &age, age_cmp);
	traval_list(my, print_fun);

	destroy_list(my);

	return 0;
}
