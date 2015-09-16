#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mylist.h>

#define NAMESIZE 32

struct stu_st {
	int id;
	char name[NAMESIZE];
	struct list_head node;
};

//gcc main.c -I.
//gcc main.c -I. -E
int main(void)
{
	//struct list_head head = { &(head), &(head) };
	LIST_HEAD(head);
	struct stu_st *data = NULL;
	struct stu_st *cur = NULL;
	struct list_head *pos = NULL;
	int i;
	char str[NAMESIZE] = "shagua5";

	for (i = 1; i <= 10; i++) {
		data = malloc(sizeof(*data));
		if (NULL == data)
			break;
		data->id = rand()%10+i;
		snprintf(data->name, NAMESIZE, "shagua%d", i);

		list_add_tail(&data->node, &head);
	}

	//for (pos = (&head)->next; pos != (&head); pos = pos->next) {
	list_for_each(pos, &head) {
		//cur = ({ const typeof( ((struct stu_st *)0)->node ) *__mptr = (pos);  \
				(struct stu_st *)( (char *)__mptr - ((size_t) &((struct stu_st *)0)->node) );});
		cur = list_entry(pos, struct stu_st, node);
		printf("%-3d%s\n", cur->id, cur->name);
	}

	//delete
	printf("*********delete********\n");
	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		if (strcmp(cur->name, str) == 0) {
			list_del(&cur->node);
			break;
		}
	}

	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		printf("%-3d%s\n", cur->id, cur->name);
	}

	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		free(cur);
	}

	return 0;
}
