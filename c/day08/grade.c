#include <stdio.h>

struct grade_st{
	int math;
	int english;
};

typedef struct stu_st{
	struct grade_st score;
	char name[32];
}stu_t;

void input_arr(stu_t *p, int num)
{
	int i;

	for (i = 0; i < num; i++) {
		p[i].score.math = rand()%100 - i;	
		p[i].score.english = rand()%100 - 10*i;	
		snprintf(p[i].name, 32, "stu%d", i+1);
	}	
}

void traval(stu_t *p, int num)
{
	int i;

	for (i = 0; i < num; i++) {
		printf("%-8s%-4d%d\n", p[i].name, ((p+i)->score).math,\
				p[i].score.english);	
	}
}

int main(void)
{
	stu_t arr[2] = {};
	
	input_arr(arr, 2);

	traval(arr, 2);

	return 0;
}
