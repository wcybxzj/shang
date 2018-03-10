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


//测试1:
//SADD 命令 插入字符串元素
//使用不可转换成整形的字符串来做为set对象的元素
//set对象底层使用hashtable
void sadd_str()
{
	char *str= "abc";
	robj *str_obj= createStringObject(str, strlen(str));
	robj *set_obj = setTypeCreate(str_obj);
	print_obj_encoding(set_obj);
}

//测试2:
//SADD 命令 插入整型元素
void sadd_int()
{
	int i;
	robj *int_obj;
	robj *set_obj;

	printf("当前 server.set_max_intset_entries:%ld\n",server.set_max_intset_entries);

	//创建set对象
	//这里set只是创建里面还没有元素
	int_obj = createStringObjectFromLongLong(1);//随便给个数字
	set_obj = setTypeCreate(int_obj);
	printf("创建set对象,里面还没有元素\n");
	print_obj_encoding(set_obj);
	printf("==========================\n");

	//插入超过5个元素就会set底层就会从intset转换成hashtable
	int arr[10]={11,22,33,44,55,66,77,88,99,100};
	for (i = 0; i <10; i++) {
		int_obj = createStringObjectFromLongLong(arr[i]);
		setTypeAdd(set_obj, int_obj);
		printf("insert one int\n");
		print_obj_encoding(set_obj);
	}
}
	int main(int argc, const char *argv[])
	{
		initServerConfig();
		createSharedObjects();

		sadd_str();
		//sadd_int();


		return 0;
	}

