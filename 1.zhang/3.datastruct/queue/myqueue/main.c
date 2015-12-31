#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct data_st {
	int id;
	int grade;
};

int main(int argc, const char *argv[])
{
	QUEUE * que;
	int i;
	struct data_st tmp;
	int idarr[] = {1,2,3,4,5,6,7,8,9};

	que = init_queue(sizeof(struct data_st));	
	if (que == NULL) 
		exit(1);

	for (i = 0; i < sizeof(idarr)/sizeof(*idarr); i++) {
		tmp.id = idarr[i];
		tmp.grade = 100 - idarr[i];		
		in_queue(que, &tmp);
	}

	while (!empty_queue(que)) {
		out_queue(que, &tmp);	
		printf("%-3d%-3d\n", tmp.id, tmp.grade);
	}

	destroy_queue(que);

	return 0;
}
