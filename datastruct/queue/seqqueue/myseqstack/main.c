#include <stdio.h>
#include <stdlib.h>

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

//用户才定义保存的数据类型所以循环也需要在用户层写
void foreach(QUEUE *ptr){
	int i, len;
	seq_queue *you = ptr;//必须转换void * 为结构体类型指针
	len = (you->rear - you->front)/you->size;

	STU *tmp_p = NULL;//偏移临时结构体指针
	STU *que_data_p = NULL;
	//que_data_p就是数组名,就是首元素的地址
	que_data_p = (STU *)(you->que_data);//必须转char * 为结构体类型指针

	////指针法
	//for (i = 0; i < len; i++) {
	//	tmp_p = que_data_p + i;
	//	printf("id:%d name %s\n", tmp_p->id, tmp_p->name);
	//}
	
	//数组法
	for (i = 0; i < len; i++) {
		printf("id:%d name %s\n", que_data_p[i].id, que_data_p[i].name);
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
	foreach(me);
	printf("============================after qsort======================\n");
	seq_queue *you = me;
	qsort(you->que_data+you->front, len_queue(me), you->size, id_cmp);
	foreach(me);

	printf("============================bsearch====================\n");
	//bsearch(const void *key, const void *base, size_t nel, size_t width, id_cmp);



	destroy_queue(me);
	return 0;
}
