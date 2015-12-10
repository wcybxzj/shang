#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_SIZE 1024

struct student_st{
	int id;
	char *name;
	int math;
	int chinese;
};

void stu_set(struct student_st *p, const struct student_st *q )
{	
	//其实当前例子可以怎么写,下面的下发是考虑了添加多个内容
	//*p = *q;
	//p->name = q->name;//两者指向同一块空间
	p->id = q->id;
	//printf("len is %d\n", strlen(q->name)+1);
	p->name = malloc(strlen(q->name)+1);
	strcpy(p->name, q->name);
	p->chinese= q->chinese;
	p->math= q->math;
}

void stu_show(struct student_st *p)
{
	printf("%d %s %d %d\n", p->id, p->name,  p->math, p->chinese);
}

void menu()
{
	printf("\n1.set 2.change name 3.show \n");
}

void stu_changename(struct student_st *stu, const char *newname){
	free(stu->name);
	stu->name = malloc(strlen(newname)+1);
	strcpy(stu->name, newname);
}

int main(int argc, const char *argv[])
{
	struct student_st stu, tmp;
	int choice;
	char *newname;
	int ret;
	while (1) {
		menu();
		ret = scanf("%d", &choice);
		if (!ret) {
			break;
		}
		switch(choice){
			case 1:
				tmp.name = malloc(MAX_SIZE);
				if (NULL == tmp.name) {
					printf("%s\n", strerror(errno));
					exit(1);
				}
				printf("plz enter for [id name math chinese]:\n");
				scanf("%d%s%d%d", &tmp.id, tmp.name, &tmp.math, &tmp.chinese);
				//printf("%d %s %d %d\n", tmp.id, tmp.name, tmp.math, tmp.chinese);
				stu_set(&stu, &tmp);//stu = tmp
				free(tmp.name);
				break;

			case 2:
				printf("ple enter new name:\n");
				newname = malloc(MAX_SIZE);
				scanf("%s", newname);
				stu_changename(&stu, newname);
				free(newname);
				break;

			case 3:
				//tmp.id =123;
				stu_show(&stu);
				//stu_show(&tmp);
				break;
			default:
				//free
				break;
		}
	}


	return 0;
}
