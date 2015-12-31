#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "seqstack.h"

#define BALL_NR 27

static int queueisok(QUEUE *ball)
{
	int i;

	for (i = ball->front; i < (ball->rear-1+QUEUEMAX)%QUEUEMAX; i = (i+1)%QUEUEMAX) {
			if (ball->queue[i] > ball->queue[i+1])
				return 0;	
	}

	return 1;
}

void traval_queue(QUEUE *ball)
{
	int i;

	for (i = ball->front; i < ball->rear; i = (i+1)%QUEUEMAX) {
		printf("%d ", ball->queue[i]);
	}	
	printf("\n");
}

int main(void)
{
	QUEUE *ball = NULL;
	STACK *onemin = NULL;
	STACK *fivemin = NULL;
	STACK *onehour = NULL;
	int i, tmp;
	int timecnt = 0;
	
	ball = init_queue();
	if (NULL == ball)
		exit(1);
	onemin = initstack();
	if (NULL == onemin)
		exit(1);
	fivemin = initstack();
	if (NULL == fivemin)
		exit(1);
	onehour = initstack();
	if (NULL == onehour)
		exit(1);

	for (i = 1; i <= BALL_NR; i++) {
		enq_queue(ball, i);	
	}

	while (1) {
		deq_queue(ball, &tmp);	
		timecnt++;
		if (get_stacknum(onemin) < 4) {
			pushstack(onemin, tmp);
		} else {
			while (!emptystack(onemin)) {
				popstack(onemin, &i);
				enq_queue(ball, i);
			}
			if (get_stacknum(fivemin) < 11) {
				pushstack(fivemin,tmp);
			} else {
				while (!emptystack(fivemin)) {
					popstack(fivemin, &i);
					enq_queue(ball, i);
				}
				if (get_stacknum(onehour) < 11) {
					pushstack(onehour, tmp);
				} else {
					while (!emptystack(onehour)) {
						popstack(onehour, &i);
						enq_queue(ball, i);
					}	
					enq_queue(ball, tmp);
					if (queueisok(ball)) {
						break;
					}
				}
			}
		}	
	}

	traval_queue(ball);
	printf("timecnt = %d\n", timecnt);

	destroy_queue(ball);
	destroystack(onemin);
	destroystack(fivemin);
	destroystack(onehour);

	exit(0);
}
