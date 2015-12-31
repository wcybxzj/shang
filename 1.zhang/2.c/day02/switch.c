#include <stdio.h>

int main(void)
{
	int grade;

	printf("请输入成绩：\n");
	scanf("%d", &grade);	

	switch (grade / 10) {
		case 10:
			printf("这都能答满\n");
		case 9:
			printf("你好屌啊！\n");
			break;
		case 8:
			printf("还不错\n");
			break;
		case 7:
			printf("马马虎虎\n");
			break;
		case 6:
			printf("小子！算你幸运\n");
			break;
		default:
			printf("没脸见爹娘\n");
			break;
	}

	return 0;
}
