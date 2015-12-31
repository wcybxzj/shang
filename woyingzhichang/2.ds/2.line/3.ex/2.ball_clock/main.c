#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "sqstack.h"
#define NUM_BALL

int check(Q* qu)
{
	int i;
	int tmp = 1;
	if (q_isempty(qu)) {
		return 0;
	}
	for (i = (qu->head+1)%MAXSIZE;
			i != (qu->tail+1)%MAXSIZE; i=(i+1)%MAXSIZE) {
		if (qu->data[i] != tmp) {
			return 0;
		}
		tmp++;
	}
	return 1;
}

int main(int argc, const char *argv[])
{
	int i = 1, t=0, t1, time=0;
	SK *one_hour = NULL, *five_min = NULL, *one_min = NULL;
	Q *qu = NULL;

	one_hour = st_create();
	five_min = st_create();
	one_min = st_create();
	qu = q_create();

	//入队
	for (i=1; i<=27; i++) {
		if (q_enqueue(qu, &i) != 0) {
			break;
		}
	}

	q_travel(qu);

	while (1) {
		//printf("queue:\n");
		//q_travel(qu);
		//printf("one_min:\n");
		//st_travel(one_min);
		//printf("five_min:\n");
		//st_travel(five_min);
		//printf("one_hour:\n");
		//st_travel(one_hour);
		//printf("---------------\n");
		q_dequeue(qu, &t);
		time++;
		if (one_min->top != 3) {//1
			st_push(one_min, &t);
		}else{
			while (!st_isempty(one_min)) {
				st_pop(one_min, &t1);
				q_enqueue(qu, &t1);
			}
			if (five_min->top!=10) {//5
				st_push(five_min, &t);
			}else{
				while (!st_isempty(five_min)) {
					st_pop(five_min, &t1);
					q_enqueue(qu, &t1);
				}
				if (one_hour->top!=10) {//one hour
					st_push(one_hour, &t);
				}else{
					while (!st_isempty(one_hour)) {
						st_pop(one_hour, &t1);
						q_enqueue(qu, &t1);
					}
					q_enqueue(qu, &t);
					q_travel(qu);
					if (check(qu)) {
						printf("%d\n", time);
						break;
					}
				}
			}

		}
	}



	q_destroy(qu);
	st_destory(one_hour);
	st_destory(five_min);
	st_destory(one_min);

	return 0;
}
