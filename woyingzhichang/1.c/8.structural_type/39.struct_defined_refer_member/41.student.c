#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMESIZE 32

struct student_st{
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
};

void stu_set(struct student_st *p, const struct student_st *q )
{
	*p = *q;
}

void stu_show(struct student_st *p)
{
	printf("%d %s %d %d\n", p->id, p->name,  p->math, p->chinese);
}

void menu()
{
	printf("\n1.set 1.change name 3.show \n");
}

void stu_changename(struct student_st *stu, const char *newname){
	strcpy(stu->name, newname);
}

int main(int argc, const char *argv[])
{
	struct student_st stu, tmp;
	int choice;
	char newname[NAMESIZE];
	int ret;
	while (1) {
		menu();
		ret = scanf("%d", &choice);
		if (!ret) {
			break;
		}
		switch(choice){
			case 1:
				printf("plz enter for [id name math chinese]:\n");
				scanf("%d%s%d%d", &tmp.id, tmp.name, &tmp.math, &tmp.chinese);
				//printf("%d %s %d %d\n", tmp.id, tmp.name, tmp.math, tmp.chinese);
				stu_set(&stu, &tmp);//stu = tmp
				break;

			case 2:
				printf("ple enter new name:\n");
				scanf("%s", newname);
				stu_changename(&stu, newname);
				break;

			case 3:
				//tmp.id =123;
				stu_show(&stu);
				//stu_show(&tmp);
				break;
		}
	}


	return 0;
}
