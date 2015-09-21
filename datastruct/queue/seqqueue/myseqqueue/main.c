#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue.h>

#define NAMESIZE 32
typedef struct stu_st{
	int id;
	char name[NAMESIZE];
} STU;

int id_cmp(const void *s1, const void *s2){
	const STU *a = s1;
	const STU *b = s2;
	return a->id - b->id;
}

//offset que_data 通过偏移量获取index
int get_index_que_data(int offset, int size, int len)
{
	int i, index=0;
	for (i=0; i<=size*len; i+=size) {
		if (offset == i) {
			return index;
		}
		index++;
	}
	return index;
}

//比较适合用指针, 用数组下标麻烦
void foreach(QUEUE *ptr){
	seq_queue *you = ptr;//必须转换void * 为结构体类型指针
	STU * tmp_stu = NULL;
	int tmp_front = you->front;

	printf("========指针方式========\n");
	while (tmp_front != you->rear) {
		tmp_stu = (STU*)(you->que_data + tmp_front);
		printf("address:%p id:%d, name:%s\n", tmp_stu, tmp_stu->id, tmp_stu->name);
		tmp_front = (tmp_front + you->size)%(QUEUEMAX * you->size);
	}

	printf("=========数组方式=========\n");
	tmp_front = you->front;
	int index = 0;
	STU *que_data_p = (STU *)(you->que_data);//必须转char * 为结构体类型指针
	while (tmp_front != you->rear) {
		index = get_index_que_data(tmp_front, you->size, QUEUEMAX);
		printf("address:%p index:%d id:%d, name:%s\n",
				&que_data_p[index], index, que_data_p[index].id, que_data_p[index].name);
		tmp_front = (tmp_front + you->size)%(QUEUEMAX * you->size);
	}
}

//1.顺序存储结构中的数据是结构体
//2.顺序存储中的分配空间是变长结构体,效率更高,且不占用空间
//3.queue.h中没有暴露结构类型
//4.qsort/bsearch
//5.支持按照id顺序插入
int main(int argc, const char *argv[])
{
	int i, j;
	int arr[] = {11,22,33,44,55,66,77,88,99,100,101,102};
	int arr2[] = {201,202,203,204,205,206,207,208,209};
	STU tmp_stu;
	QUEUE *me = NULL;

	printf("----------------------入队-------------------------\n");
	me = init_queue(sizeof(STU));
	for (i = 0; i <sizeof(arr)/sizeof(*arr) ; i++) {
		tmp_stu.id = arr[i];
		snprintf(tmp_stu.name, NAMESIZE, "大葱%d", arr[i]);
		if (FAIL == enq_queue(me, &tmp_stu)) {
			printf("入队fail id:%d name %s\n", tmp_stu.id, tmp_stu.name);
			break;
		}else{
			printf("入队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
		}
	}
	printf("----------------------出队---------------------------\n");
	while (1) {
		if (FAIL == deq_queue(me, &tmp_stu)) {
			break;
		}
		printf("出队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
	}
	printf("=====================边出边入============================\n");
	for (j = 0; j <3; j++) {
		for (i = 0; i <sizeof(arr)/sizeof(*arr) ; i++) {
			tmp_stu.id = arr[i];
			snprintf(tmp_stu.name, NAMESIZE, "大葱%d", arr[i]);
			if (FAIL == enq_queue(me, &tmp_stu)) {
				printf("入队fail id:%d name %s\n", tmp_stu.id, tmp_stu.name);
				break;
			}else{
				printf("入队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
			}
		}
		
		for (i = 0; i < 3; i++) {
			deq_queue(me, &tmp_stu);
			printf("出队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
		}
	}

	printf("============================before qsort======================\n");
	for (i = 0; i < 3; i++) {
		deq_queue(me, &tmp_stu);
		printf("出队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
	}
	for (i = 0; i <sizeof(arr2)/sizeof(*arr2) ; i++) {
		tmp_stu.id = arr2[i];
		snprintf(tmp_stu.name, NAMESIZE, "大葱%d", arr2[i]);
		if (FAIL == enq_queue(me, &tmp_stu)) {
			printf("入队fail id:%d name %s\n", tmp_stu.id, tmp_stu.name);
			break;
		}else{
			printf("入队ok id:%d name %s\n", tmp_stu.id, tmp_stu.name);
		}
	}
	printf("======================foreach=============================\n");
	foreach(me);
	return 0;
	//printf("============================after qsort======================\n");

	//seq_queue *you = me;
	//qsort(you->que_data+you->front, len_queue(me), you->size, id_cmp);
	//foreach(me);

	//printf("============================bsearch====================\n");
	//STU *tmp_stu_bsearch=NULL;
	//tmp_stu.id = 22;
	//snprintf(tmp_stu.name, NAMESIZE, "大葱%d", 22);

	//tmp_stu_bsearch = bsearch( &tmp_stu, you->que_data+you->front, len_queue(me), you->size, id_cmp);

	//if (tmp_stu_bsearch) {
	//	printf("二分查找到元素 id: %d name:%s\n", tmp_stu_bsearch->id, tmp_stu_bsearch->name);
	//}else{
	//	printf("二分查没找到元素\n");
	//}

	destroy_queue(me);
	return 0;
}
