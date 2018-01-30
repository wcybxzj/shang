
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

void _serverAssert(char *estr, char *file, int line) {
	printf("estr:%s,file:%s, line:%d\n",estr, file, line);

}

void _serverAssertWithInfo(client *c, robj *o, char *estr, char *file, int line) {
	UNUSED(c);
	UNUSED(o);
    _serverAssert(estr,file,line);
}


void serverLogHexDump(int level, char *descr, void *value, size_t len) {
	UNUSED(level);
    char buf[65], *b;
    unsigned char *v = value;
    char charset[] = "0123456789abcdef";

    //serverLog(level,"%s (hexdump of %zu bytes):", descr, len);
    printf("%s (hexdump of %zu bytes):", descr, len);
    b = buf;
    while(len) {
        b[0] = charset[(*v)>>4];
        b[1] = charset[(*v)&0xf];
        b[2] = '\0';
        b += 2;
        len--;
        v++;
        if (b-buf == 64 || len == 0) {
            //serverLogRaw(level|LL_RAW,buf);
			printf("%s\n",buf);
            b = buf;
        }
    }
    //serverLogRaw(level|LL_RAW,"\n");
	printf("\n");
}

