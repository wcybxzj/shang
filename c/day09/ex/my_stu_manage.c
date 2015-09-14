#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void display_menu();
enum menu_list {ADD=1, DEL, MOD, SCAN, CLEAN, QUIT};

typedef struct student{
	int id;
	char name[32];
	int grade;
}stu_t;

typedef int (*com_func)(const void *, const void *);

void clean(stu_t **ptr, int *stu_num);
void scan(stu_t *ptr, int stu_num);
void add(stu_t **ptr, stu_t tmp, int *num);
void del(stu_t **ptr, int *stu_num, void *key , com_func func);

int id_compare(const void *key, const void * data){
	const int *id_val = key;
	const stu_t *d  = data;
	return *id_val - d->id;
}

int name_compare(const void * key, const void * data){
	const char *name_val = key ;
	const stu_t *d  = data;
	return strcmp(name_val, d->name);
}

int main(int argc, const char *argv[])
{
	int tmp_id;
	int num = 0;//id计数
	int stu_num = 0;//内存使用计数
	stu_t tmp;
	stu_t *p = NULL;
	enum menu_list choose;

	while (1) {
		system("clear");
		display_menu();
		printf("请输入你的选项:\n");
		scanf("%d", &choose);

		switch(choose){
			case ADD:
				tmp.id = num;
				snprintf(tmp.name, 32, "姓名%d", num);
				tmp.grade = num+1;
				printf("num:%d id:%d name:%s grade:%d \n",
						num,tmp.id , tmp.name, tmp.grade);
				add(&p, tmp, &stu_num);
				num++;
				sleep(1);
				break;
			case DEL:
				printf("plz enter id\n");
				scanf("%d", &tmp_id);
				del(&p, &stu_num, &tmp_id, id_compare);
				sleep(1);
				break;
			case MOD:
				break;
			case SCAN:
				printf("scan\n");
				scan(p, stu_num);
				sleep(3);
				break;
			case CLEAN:
				clean(&p, &stu_num);
				break;
			case QUIT:
				break;
		}
	}
	return 0;
}

void add(stu_t **ptr, stu_t tmp, int *num)
{
	stu_t *p = malloc(sizeof(stu_t) * (*num+1));
	if (NULL == p) {
		exit(1);
	}
	memset(p, 0x00, sizeof(stu_t) * (*num+1));
	memcpy(p, *ptr, sizeof(stu_t) * (*num));
	memcpy(p+(*num), &tmp, sizeof(stu_t));

	(*num)++;
	free(*ptr);
	*ptr = p;
}

void scan(stu_t *ptr, int stu_num){
	int i;
	for (i = 0; i < stu_num; i++) {
		printf("id:%d name:%s grade:%d \n", (ptr+i)->id, (ptr+i)->name, (ptr+i)->grade);
	}
}

void display_menu() {
	printf("1 add, 2 del, 3 mod, 4 scan, 5 clean, 6 quit\n");
}

void del(stu_t **ptr, int *stu_num, void *key , com_func func){
	int i;
	int is_match = 0;
	for (i = 0; i < *stu_num; i++) {
		if(!func(key, *ptr+i)){
			is_match = 1;
			break;
		}
	}
	if (is_match) {
		if (i == 0 && *stu_num == 1) {
			printf("只有一个，且要删除之\n");
			clean(ptr, stu_num);
			return;
		}
		if(i != *stu_num-1){
			printf("要删除的不是最后一个\n");
			memmove(*ptr+i, *ptr+i+1, (*stu_num-i-1)*sizeof(stu_t));
		}
		(*stu_num)--;
		*ptr = realloc(*ptr, *stu_num * sizeof(stu_t));
	}else{
		printf("del have no match element\n");
	}
}

void clean(stu_t **ptr, int *stu_num) {
	free(*ptr);
	*ptr = NULL;
	*stu_num = 0;
}

