#include <stdio.h>
#include <stdlib.h>
#define NUM 8
#define KILL_NUM 3

typedef struct _NODE {
	int num;
	struct _NODE *next;
} NODE;

NODE *list_insert(int num){
	int i;
	NODE *head= NULL, *me = NULL, *tmp_head = NULL;
	head = malloc(sizeof(*head));
	head->num = 1;
	head->next = head;
	tmp_head = head;
	for (i = 2; i <= num; i++) {
		me = malloc(sizeof(*me));
		me->num = i;
		tmp_head->next = me;
		me->next = head;
		tmp_head = me; 
	}
	return head;
}

void list_display(NODE *list){
	NODE *tmp = list;
	while (tmp) {
		printf("%d ", tmp->num);
		tmp = tmp->next;
		if (tmp == list) {
			break;
		}
	}
	printf("\n");
}

//假如要到3删除，实际转化为到2删除下一个.避免到了3无法找前驱
void josufu_kill(NODE *list, int kill_num){
	NODE *tmp = list, *tmp_next = NULL;
	int tmp_num = kill_num-1;

	while (tmp->next != tmp) {
		tmp_num--;
		if (tmp_num==0) {
			tmp_next = tmp->next;
			tmp->next = tmp->next->next;
			free(tmp_next);
			tmp_num = kill_num-1;
		}
		tmp = tmp->next;
	}
	printf("survior is %d\n", tmp->num);
}

//无头单循环链表实现约瑟夫环
int main(int argc, const char *argv[])
{
	NODE *list;
	list = list_insert(NUM);
	list_display(list);
	josufu_kill(list, KILL_NUM);
	return 0;
}
