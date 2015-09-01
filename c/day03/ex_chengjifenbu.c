#include <stdio.h>
#include <time.h>

#define PERSON_NUM 20

int main(void)
{
	srand(time(NULL));
	int cnt[11] = {};		
	int score[PERSON_NUM] = {};
	int i, j;

	for (i = 0; i < PERSON_NUM; i++) {
		score[i] = rand() % 101;
		cnt[score[i]/10]++;
		printf("%d ", score[i]);
	}
	printf("\n");

	printf("此班学生的成绩分布如下\n");
	printf("100    :");
	for (i = 0 ;i < cnt[10]; i++) 
		printf("* ");
	printf("\n");
	for (i = 9; i >= 0; i --) {
		printf("%2d ~ %2d:", i*10, i*10+9);
		for (j = 0; j < cnt[i]; j++) {
			printf("\33[43m  ");
		}
		printf("\33[0m\n\n");
	}

	return 0;
}

