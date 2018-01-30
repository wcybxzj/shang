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

	int i, ret;
	robj *key_o;
	robj *val_o;
	char *key_str = "key_zset";
	robj* zobj;
	robj* zobj1;
	INFO info_arr[NUM];
	zobj = createZsetObject();//skiplist做zset底层

	for (i = 0; i < NUM; i++) {
		info_arr[i].elem_str = genstr("elem", i);
		info_arr[i].score = i;
		info_arr[i].elem_o  = createStringObject(info_arr[i].elem_str, strlen(info_arr[i].elem_str));
		info_arr[i].score_o = NULL;
		ybx_zaddGenericCommand(zobj, info_arr[i].score,info_arr[i].elem_o);
	}

	//添加键值对
	key_o  = createStringObject(key_str, strlen(key_str));
	dbAdd(server.db+0,key_o, zobj);

	//查找键值对
	val_o= lookupKeyWrite(server.db+0, key_o);
	ybx_zrangeGenericCommand(val_o, 5, -1);

	printf("=================================================\n");

	//修改键值对
	zobj1 = createZsetObject();//skiplist做zset底层
	for (i = 0; i < NUM; i++) {
		info_arr[i].elem_str = genstr("elem", i+1);
		info_arr[i].score = i+1;
		info_arr[i].elem_o  = createStringObject(info_arr[i].elem_str, strlen(info_arr[i].elem_str));
		info_arr[i].score_o = NULL;
		ybx_zaddGenericCommand(zobj1, info_arr[i].score,info_arr[i].elem_o);
	}
	setKey(server.db+0, key_o, zobj1);

	//查找键值对
	val_o= lookupKeyWrite(server.db+0, key_o);
	ybx_zrangeGenericCommand(val_o, 5, -1);
	ret = dbDelete(server.db+0, key_o);
	if (ret == 1) {
		printf("delete key OK\n");
	}else{
		printf("delete key fail\n");
	}

	//可以看到值对象还是存在,dbDelete值是引用计数减1
	printf("==================================================\n");
	ybx_zrangeGenericCommand(val_o, 5, -1);

}
