#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

//libevent中使用的尾队列
/*
  定义一个结构体，它只是尾队列的一个元素
  它必须包含一个TAILQ_ENTRY来指向上一个和下一个元素
*/
struct tailq_entry {
	int value;
	TAILQ_ENTRY(tailq_entry) entries;
};

//定义队列的头部
//定义结构体 struct tailq_head
TAILQ_HEAD(tailq_head, tailq_entry) my_tailq_head;

int main(int argc, char  *argv[])
{
	//定义一个结构体指针
	struct tailq_entry *item;
	//定义另外一个指针
	struct tailq_entry *tmp_item;
	
	//初始化队列
	TAILQ_INIT(&my_tailq_head);

	int i;
	//在队列里添加10个元素
	for(i=0; i<10; i++) {
		//申请内存空间
		item = malloc(sizeof(*item));
		if (item == NULL) {
			perror("malloc failed");
			exit(-1);
		}
		//设置值
		item->value = i;

		/*
		   将元素加到队列尾部
		   参数1：指向队列头的指针
		   参数2：要添加的元素
		   参数3：结构体的变量名
		*/
		TAILQ_INSERT_TAIL(&my_tailq_head, item, entries);
	}

	//遍历队列
	printf("Forward traversal: ");
	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		printf("%d ",item->value);
	}
	printf("\n");

	//添加一个新的元素
	printf("Adding new item after 5: ");
	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		if (item->value == 5) {
			struct tailq_entry *new_item = malloc(sizeof(*new_item));
			if (new_item == NULL) {
				perror("malloc failed");
				exit(EXIT_FAILURE);
			}
			new_item->value = 10;
			//插入一个元素
			TAILQ_INSERT_AFTER(&my_tailq_head, item, new_item, entries);
			break;
		}
	}
	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		printf("%d ", item->value);
	}
	printf("\n");

	//TAILQ_PREV
	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		if (item->value == 5) {
			item  = TAILQ_PREV(item, tailq_head, entries);
			printf("the entry before 5 last is %d\n", item->value);
			break;
		}
	}

	//TIALQ_LAST
	item = TAILQ_LAST(&my_tailq_head, tailq_head);
	if (item != NULL) {
		printf("the last is:%d\n",item->value);
	}

	//删除一个元素
	printf("Deleting item with value 3: ");
	for(item = TAILQ_FIRST(&my_tailq_head); item != NULL; item = tmp_item) {
		if (item->value == 3) {
			//删除一个元素
			TAILQ_REMOVE(&my_tailq_head, item, entries);
			//释放不需要的内存单元
			free(item);
			break;
		}
		tmp_item = TAILQ_NEXT(item, entries);
	}

	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		printf("%d ", item->value);
	}
	printf("\n");

	printf("Deleting item with value 8: ");
	for(item = TAILQ_FIRST(&my_tailq_head); item != NULL;  item = TAILQ_NEXT(item, entries)) {
		if (item->value == 8) {
			//删除一个元素
			TAILQ_REMOVE(&my_tailq_head, item, entries);
			//释放不需要的内存单元
			free(item);
			break;
		}
	}
	TAILQ_FOREACH(item, &my_tailq_head, entries) {
		printf("%d ", item->value);
	}
	printf("\n");

	while (item = TAILQ_FIRST(&my_tailq_head)) {
		TAILQ_REMOVE(&my_tailq_head, item, entries);
		free(item);
	}

	//查看是否为空
	if (!TAILQ_EMPTY(&my_tailq_head)) {
		printf("tail queue is  NOT empty!\n");
	}

	return 0;
}
