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


int main(int argc, const char *argv[])
{
	initServerConfig();
	createSharedObjects();
	char *str1="abc";
	char str2[100]={'\0'};
	memset(str2,'x',99);
	robj* embstr_str_obj = createStringObject(str1,strlen(str1));
	robj* raw_str_obj = createStringObject(str2,strlen(str2));
	return 0;
}

