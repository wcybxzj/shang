#ifndef __14_TEST_T_Z_SET_h
#define __14_TEST_T_Z_SET_h

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

void ybx_zaddGenericCommand(robj *zobj, double score, robj *elem_o);
void ybx_zrangeGenericCommand(robj *zobj, long start, long end);

#endif
