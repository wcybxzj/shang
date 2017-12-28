
#include "server.h"
//#include "sha1.h"   /* SHA1 is used for DEBUG DIGEST */
#include "crc64.h"

#include <arpa/inet.h>
#include <signal.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#include <ucontext.h>
#include <fcntl.h>
#include "bio.h"
#endif /* HAVE_BACKTRACE */

#ifdef __CYGWIN__
#ifndef SA_ONSTACK
#define SA_ONSTACK 0x08000000
#endif
#endif
//临时自己写的
void _serverPanic(char *msg, char *file, int line) {
	printf("msg:%s,file:%s, line:%d\n",msg, file, line);
}
