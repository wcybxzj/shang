#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stu_st {
	int id;
	char sex;
	char name[32];
};

int main(void)
{
	struct stu_st person = {1, 'f', "mwt"};	
	struct stu_st *p = NULL;

	person.sex = 'm';

	printf("%d %s %c\n", person.id,person.name,person.sex);

	p = malloc(sizeof(*p));

	memset(p, 0x00, sizeof(*p));

	p->id = 2;
	p->sex = 'f';
	strcpy(p->name , "liulu");

	printf("%d %s %c\n", p->id,(*p).name,p->sex);

	return 0;
}
