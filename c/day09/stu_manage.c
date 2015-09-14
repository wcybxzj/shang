#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OK 		0
#define FAIL	1

enum menu_en{INSERT = 1, DELETE, UPDATE, SCAN, QUIT=9, CLEAN = 10};

typedef int (*cmpare_t)(const void *, const void *);

typedef struct stu_st {
	int id;
	char name[32];
	int grade;
}Stu_t;

void clean(Stu_t **ptr,  int* stu_num);
int delete(Stu_t **ptr, int *num, const void *key, cmpare_t op);
int update(Stu_t **ptr, int num, Stu_t stu, const void *key, cmpare_t op);
int insert(Stu_t **ptr, Stu_t stu, int *num);
void scan(Stu_t *ptr, int num);
void menulist(void);

int id_compare(const void *key, const void *data)
{
	const int *k = key;
	const Stu_t *d = data;

	return *k-d->id;	
}

int name_compare(const void *key, const void *data)
{
	const char *k = key;
	const Stu_t *d = data;

	return strcmp(k, d->name);
}

int main(void)
{
	Stu_t *p = NULL;
	int stu_num = 0;
	enum menu_en choose;
	Stu_t tmp;
	int i = 1;
	int id;

	while (1) {
		system("clear");
		menulist();
		printf("输入你的选择\n");
		scanf("%d", &choose);
		switch (choose) {
			case INSERT:
				tmp.id = i;
				snprintf(tmp.name, 32, "stu%d", i);
				tmp.grade = 100-i;
				i++;
				printf("[行号%d]id: %d name:%s grade:%d\n", __LINE__, tmp.id, tmp.name, tmp.grade);
				sleep(1);
				insert(&p, tmp, &stu_num); break;
			case DELETE:
				//delete(&p, &stu_num, &id, id_compare); break;
				delete(&p, &stu_num, "stu3", name_compare); break;
			case UPDATE:
				id = 2;
				tmp.id = 11;
				strncpy(tmp.name,"new", 32);
				tmp.grade = 100;	
				update(&p, stu_num, tmp, &id, id_compare); break;
			case SCAN:
				scan(p, stu_num); 
				sleep(2);
				break;
			case QUIT:
				goto quit;
			case CLEAN:
				clean(&p, &stu_num);
				break;
			default:
				goto error;
				break;
		}	
	}

	return 0;
error:
	return 1;
quit:
	printf("Bye-Bye\n");
	free(p);
	return 0;
}

void menulist(void)
{
	printf("1.插入学生信息    2.删除学生信息   \n3.更改学生信息    4.查看信息    9.退出系统\n");	
}

int insert(Stu_t **ptr, Stu_t stu, int *num)
{
	Stu_t *p = malloc(sizeof(Stu_t)*(*num+1));		
	if (NULL == p) {
		return FAIL; 
	}
	memset(p, 0x00, sizeof(Stu_t)*(*num+1));
	memcpy(p, *ptr, sizeof(Stu_t) * (*num));
	memcpy(p+(*num), &stu, sizeof(stu));

	(*num)++;
	free(*ptr);
	*ptr = p;

	return 0;
}

int delete(Stu_t **ptr, int *num, const void *key, cmpare_t op)
{
	int i;
	for (i = 0; i < *num; i++) {
		if (!op(key, (*ptr)+i))	
			break;
	}	
	if (i == *num) 
		return FAIL;
	memmove(*ptr + i, *ptr+i+1, (*num-i-1)*sizeof(Stu_t));

	(*num)--;
	*ptr = realloc(*ptr, (*num)*sizeof(Stu_t));
	return OK;
}

int update(Stu_t **ptr, int num, Stu_t stu, const void *key, cmpare_t op)
{
	int i;
	for (i = 0; i < num; i++) {
		if (!op(key, (*ptr)+i))
			break;
	}
	if (i == num)
		return FAIL;
	memcpy(*ptr+i, &stu, sizeof(stu));	
	return OK;
}

void scan(Stu_t *ptr, int num)
{
	int i;

	for (i = 0; i < num; i++) {
		printf("%-4d%-8s%d\n", (ptr+i)->id, (ptr+i)->name, (ptr+i)->grade);
	}
}

void clean(Stu_t **ptr,  int* stu_num){
	free(*ptr);
	*ptr = NULL;
	*stu_num = 0;
}
