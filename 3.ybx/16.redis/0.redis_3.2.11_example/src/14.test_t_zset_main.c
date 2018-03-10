#include "14.test_t_zset.h"
#define NUM 10

typedef struct _INFO {
	char *elem_str;
	double score;
	robj *elem_o;
	robj *score_o;
} INFO;

int main(int argc, const char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	init();

	int i;
	robj* zobj;
	INFO info_arr[NUM];
	zobj = createZsetObject();//skiplist做底层
	//zobj = createZsetZiplistObject();//ziplist做底层

	for (i = 0; i < NUM; i++) {
		info_arr[i].elem_str = genstr("elem", i);
		info_arr[i].score = i;
		info_arr[i].elem_o  = createStringObject(info_arr[i].elem_str, strlen(info_arr[i].elem_str));
		info_arr[i].score_o = NULL;
		ybx_zaddGenericCommand(zobj, info_arr[i].score,info_arr[i].elem_o);
	}

	ybx_zrangeGenericCommand(zobj, 5, -1);
}
