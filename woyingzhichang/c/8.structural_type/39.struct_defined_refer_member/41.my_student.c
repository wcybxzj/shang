#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define NAMESIZE 32

struct list_st{
	int num;
	struct student_st *p;
};

struct student_st{
	int id;
	char name[NAMESIZE];
};

void stu_set(struct student_st *p, const struct student_st *q )
{
	*p = *q;
}

void stu_show(struct list_st* list_p)
{
	int i;
	for (i = 0; i < list_p->num; i++) {
		printf("%d %s\n",
				list_p->p[i].id,
				list_p->p[i].name);
	}
}

void menu()
{
	printf("\n1.add 2.del 3.edit 4.show all\n");
}

void stu_changename(struct student_st *stu, const char *newname){
	strcpy(stu->name, newname);
}

void delete(struct list_st *list, int choice_id)
{
	int i;
	for (i = 0; i < list->num; i++) {
		if ((list->p+i)->id == choice_id) {
			list->num--;
			if (list->num==0) {
				free(list->p);
				list->p = NULL;
			}else if (i == 0) {
				memmove(list->p, list->p+1, sizeof(struct student_st)*(list->num));
				list->p= realloc(list->p, (list->num)* sizeof(struct student_st));
			}else if(i == list->num){
				list->p = realloc(list->p, (list->num)* sizeof(struct student_st));
			}else{
				memmove(list->p+i, list->p+i+1, 
						(list->num-i)*sizeof(struct student_st));
				list->p= realloc(list->p, (list->num)* sizeof(struct student_st));
			}
			break;
		}
	}
}

void edit(struct list_st *list, int choice_id){
	int i;
	for (i = 0; i < list->num; i++) {
		if ((list->p+i)->id == choice_id) {
			scanf("%s", (list->p+i)->name );
			break;
		}
	}
}

int main(int argc, const char *argv[])
{
	struct list_st list;
	struct student_st tmp;
	int choice, choice_id;
	char newname[NAMESIZE];
	int ret;

	list.num = 0;
	list.p = NULL;

	while (1) {
		menu();
		ret = scanf("%d", &choice);
		if (!ret) {
			break;
		}
		switch(choice){
			case 1://add
				list.num++;
				list.p = realloc(list.p, list.num* sizeof(struct student_st));
				printf("plz enter for [id name]:\n");
				scanf("%d%s", &((list.p+list.num-1)->id), (list.p+list.num-1)->name);
				break;

			case 2://delete
				printf("ple enter id:\n");
				ret = scanf("%d", &choice_id);
				if (ret != 1) {
					break;
				}
				delete(&list, choice_id);
				break;

			case 3:
				printf("ple enter id:\n");
				ret = scanf("%d", &choice_id);
				if (ret != 1) {
					break;
				}
				edit(&list, choice_id);
				break;
			case 4:
				//tmp.id =123;
				stu_show(&list);
				//stu_show(&tmp);
				break;
		}
	}


	return 0;
}
