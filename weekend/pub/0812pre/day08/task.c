#include <stdio.h>

struct person_st {
	int id;
	char sex;
	char name[32];
	int math;
	int english;
};

int main(void)
{
	struct person_st arr[3];
	int i;
	char ch;

	for (i = 0; i < 3; i++) {
		system("clear");
		printf("请输入%d学生的id:\n", i+1);	
		scanf("%d", &arr[i].id);
		printf("姓名:\n");
		scanf("%s", arr[i].name);
		while (getchar() != '\n');
#if 0
		while (1)
		{
			scanf("%c", &ch);
			if (ch == '\n')
				break;
		}
#endif
		printf("男的女的:");
		scanf("%c", &arr[i].sex);
		printf("数学成绩和英语成绩:\n");
		scanf("%d%d", &arr[i].math, &arr[i].english);
	}

	system("clear");
	printf("name   sex  id  math  english\n");
	for (i = 0; i < 3; i++) {
		printf("%-7s%-5c%-4d%-6d%d\n", arr[i].name, arr[i].sex,\
				arr[i].id, arr[i].math, arr[i].english);
	}
	
	printf("*********数学及格的有**********\n");
	for (i = 0; i < 3; i++) {
		if (arr[i].math >= 60) {
			printf("**********%s\n", arr[i].name);
		}
	}

	return 0;
}
