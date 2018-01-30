#include "server.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>
#include "intset.h"

#define NUM 10
typedef struct _INFO {
	char *key;
	char *val;
} INFO;

typedef struct _INFO_OBJ {
	robj *key_o;
	robj *val_o;
} INFO_OBJ;

void ybx_hsetCommand(robj *o , robj*key_o, robj* val_o)
{
	int update;
	update = hashTypeSet(o,key_o,val_o);
	if (update) {
		printf("update\n");
	}else{
		printf("insert\n");
	}
}

void ybx_hgetCommand(robj *o, robj* key)
{
	char tmp[100]={};
	int ret;
	// 对象不存在
	if (o == NULL) {
		printf("object is NULL\n");
		return;
	}

	// OBJ_ENCODING_ZIPLIST编码
	if (o->encoding == OBJ_ENCODING_ZIPLIST) {
		//printf("当前哈希底层为ZIPLIST\n");
		unsigned char *vstr = NULL;
		unsigned int vlen = UINT_MAX;
		long long vll = LLONG_MAX;
		// 从ziplist中取出值
		ret = hashTypeGetFromZiplist(o, key, &vstr, &vlen, &vll);
		if (ret < 0) {
			printf("cant not get element form ziplist\n");
		} else {
			if (vstr) {
				memcpy(tmp, vstr, vlen);
				printf("%s\n",tmp);
			} else {
				printf("%lld\n",vll);
			}
		}
		// OBJ_ENCODING_HT编码
	} else if (o->encoding == OBJ_ENCODING_HT) {
		//printf("当前哈希底层为HT\n");
		robj *value;
		ret = hashTypeGetFromHashTable(o, key, &value);
		if (ret < 0) {
			printf("can not get val from hashtable\n");
		} else {
			if (value->encoding==OBJ_ENCODING_INT) {
				printf("%lld\n",value->ptr);
			}else{
				printf("%s\n",value->ptr);
			}
		}
	} else {
		serverPanic("Unknown hash encoding");
	}
}

int main(int argc, const char *argv[])
{
	init();
	int i;
	robj *o;
	robj* key_o;
	robj* val_o;

	o = createHashObject();

	INFO info_arr[NUM];
	INFO_OBJ info_obj_arr[NUM];
	//值为字符
	for (i = 0; i < 5; i++) {
		info_arr[i].key = genstr("key", i);
		info_arr[i].val = genstr("val", i);
		info_obj_arr[i].key_o =key_o = createStringObject(info_arr[i].key, strlen(info_arr[i].key));
		info_obj_arr[i].val_o =val_o = createStringObject(info_arr[i].val, strlen(info_arr[i].val));
		ybx_hsetCommand(o, key_o, val_o);
	}

	//值为数字
	for (i = 5; i < NUM; i++) {
		info_arr[i].key = genstr("key", i);
		info_arr[i].val = i;
		info_obj_arr[i].key_o =key_o = createStringObject(info_arr[i].key, strlen(info_arr[i].key));
		info_obj_arr[i].val_o =val_o = createStringObjectFromLongLong(i);
		ybx_hsetCommand(o, key_o, val_o);
	}


	for (i = 0; i < NUM; i++) {
		ybx_hgetCommand(o, info_obj_arr[i].key_o);
	}

}
