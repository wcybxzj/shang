#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	//12小时一天
	//求回到原来状态要多少天
	QUEUE *que = NULL;
	STACK *one_stack = NULL;//4进1
	STACK *five_stack = NULL;//11进1
	STACK *60_stack = NULL;//11进1

	que = init_queue();
	one_stack  = init_stack(4);
	five_stack = init_stack(11);
	sixty_stack   = init_stack(11);


	int i;
	for (i = 1; i <= 27; i++) {
		if (OK == enq_queue(que, i)) {
			printf("enq ok\n");
		}else{
			printf("enq failed\n");
			break;
		}
	}



	return 0;
}
