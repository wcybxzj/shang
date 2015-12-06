#include <stdio.h>


struct birthday{
	int year;
	int month;
	int day;
};

struct student_st{
	char *name;
	struct birthday birth;
};


int main(int argc, const char *argv[])
{
	int i;
	struct student_st s1= {"ybx", {1999,9,9}};
	printf("%s %d %d %d\n", s1.name, s1.birth.year, s1.birth.month, s1.birth.day);

	struct student_st s2 ={.name="wc"};
	printf("%s %d %d %d\n", s2.name, s2.birth.year, s2.birth.month, s2.birth.day);

	struct student_st *p = &s1;
	printf("%s %d %d %d\n", p->name, p->birth.year, p->birth.month, p->birth.day);
	printf("%s %d %d %d\n", (*p).name, (*p).birth.year, (*p).birth.month, (*p).birth.day);

	struct student_st arr[2] = {
		{"ybx",{1999,1,1}},
		{"wc", {2000,2,2}}
	};
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++) {
		printf("%s %d %d %d\n", arr[i].name, arr[i].birth.year, arr[i].birth.month, arr[i].birth.day);
	}
	
	p = arr;//arr = &arr[0]
	for (i = 0; i < sizeof(arr)/sizeof(*arr); i++, p++) {
		printf("%s %d %d %d\n", p->name, p->birth.year, p->birth.month, p->birth.day);
	}

	return 0;
}
