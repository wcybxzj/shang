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

int err;

int main(int argc, const char *argv[])
{
	initServerConfig();
	createSharedObjects();
	int fill =-2;
	int compress = 0;
	robj * obj1;
	robj * obj2;

	char *str1 = "hello";
	obj1 = createStringObject(str1, strlen(str1));
	char *str2 = "cat";
	obj2 = createStringObject(str2, strlen(str2));

	robj *qlobj = createQuicklistObject();
	quicklistSetOptions(qlobj->ptr,fill,compress);
	listTypePush(qlobj,obj1,LIST_HEAD);
	listTypePush(qlobj,obj2,LIST_HEAD);


	robj * obj3;
	robj * obj4;
	obj3 = listTypePop(qlobj,LIST_HEAD);
	printf("%s\n",obj3->ptr);
	obj4 = listTypePop(qlobj,LIST_HEAD);
	printf("%s\n",obj4->ptr);

	return 0;
}

