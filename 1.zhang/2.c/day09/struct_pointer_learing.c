#include <stdio.h>
#include <string.h>

typedef struct stu_st{
	int id;
	char name[32];
	int ch;
	int eng;
}stu_t;

void func(stu_t t)
{
	printf("id:%d\n", t.id);
	printf("name:%s\n", t.name);
	printf("ch:%d\n", t.ch);
	printf("eng:%d\n", t.eng);
}

void change(stu_t *t)
{
	t->id = 123;
	strcpy(t->name, "名字改了");
	t->ch = 60;
	t->eng = 100;
}

int main(int argc, const char *argv[])
{
	stu_t s1 = {1, "zhanghuan", 90, 99};
	func(s1);
	printf("--------------\n");
	change(&s1);
	printf("--------------\n");
	func(s1);

	printf("------为了实现通过地址获取结构体数据,需要先修改指针类型-----\n");
	stu_t *p = &s1;
	void *tmp = p;
	stu_t *p1= tmp;
	printf("%s\n",p1->name);
	return 0;
}
