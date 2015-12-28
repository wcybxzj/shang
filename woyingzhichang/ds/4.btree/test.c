#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define NAMESIZE 32
typedef struct _SCORE {
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
} SCORE_ST;

void myprint(const void *data)
{
	const SCORE_ST * cur = data;
	printf("%d %s %d %d\n",\
			cur->id, cur->name, cur->math, cur->chinese);
}

int main(int argc, const char *argv[])
{
	Q* me;
	me = q_create(sizeof(SCORE_ST));
	if (NULL == me) {
		exit(1);
	}

	int i;
	SCORE_ST tmp;
	for (i = 0; i < 10; i++) {
		tmp.id = i;
		snprintf(tmp.name, NAMESIZE, "stu%d", i);
		tmp.math = rand() % 100;
		tmp.chinese= rand() % 100;
		if (q_enqueue(me, &tmp) == 0) {
			printf("enq ok\n");
			myprint(&tmp);
		}else{
			printf("enq failed\n");
			myprint(&tmp);
			break;
		}
		
	}

	printf("----------------------------------\n");

	while (1) {
		if (q_dequeue(me, &tmp)) {
			printf("deq is ok\n");
			myprint(&tmp);
		}else{
			printf("deq is failed\n");
			break;
		}
	}

	q_destroy(me);
	return 0;
}
