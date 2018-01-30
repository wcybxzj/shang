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

static void ok(void) {
    printf("OK\n");
}

static long long usec(void) {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000000)+tv.tv_usec;
}

#define assert(_e) ((_e)?(void)0:(_assert(#_e,__FILE__,__LINE__),exit(1)))
static void _assert(char *estr, char *file, int line) {
    printf("\n\n=== ASSERTION FAILED ===\n");
    printf("==> %s:%d '%s' is not true\n",file,line,estr);
}

//就是说明intset中的整数是从小到大排列的
void checkConsistency(intset *is) {
	int i;
	for (i = 0; i < (intrev32ifbe(is->length)-1); i++) {
		uint32_t encoding = intrev32ifbe(is->encoding);
		if (encoding == INTSET_ENC_INT16) {
			int16_t *i16 = (int16_t*)is->contents;
			assert(i16[i] < i16[i+1]);
		} else if (encoding == INTSET_ENC_INT32) {
			int32_t *i32 = (int32_t*)is->contents;
			assert(i32[i] < i32[i+1]);
		} else {
			int64_t *i64 = (int64_t*)is->contents;
			assert(i64[i] < i64[i+1]);
		}
	}
}

int main(int argc, const char *argv[])
{
	uint8_t success;
	int i;
	intset *is;
	int tmp;

	//printf("INT16_MIN:%d\n",INT16_MIN);
	//printf("INT16_MAX:%d\n",INT16_MAX);
	//printf("INT32_MIN:%d\n",INT32_MIN);
	//printf("INT32_MAX:%d\n",INT32_MAX);

	printf("Value encodings: "); {
		assert(_intsetValueEncoding(-32768) == INTSET_ENC_INT16);
		assert(_intsetValueEncoding(+32767) == INTSET_ENC_INT16);
		assert(_intsetValueEncoding(-32769) == INTSET_ENC_INT32);
		assert(_intsetValueEncoding(+32768) == INTSET_ENC_INT32);
		assert(_intsetValueEncoding(-2147483648) == INTSET_ENC_INT32);
		assert(_intsetValueEncoding(+2147483647) == INTSET_ENC_INT32);
		assert(_intsetValueEncoding(-2147483649) == INTSET_ENC_INT64);
		assert(_intsetValueEncoding(+2147483648) == INTSET_ENC_INT64);
		assert(_intsetValueEncoding(-9223372036854775808ull) == INTSET_ENC_INT64);
		assert(_intsetValueEncoding(+9223372036854775807ull) == INTSET_ENC_INT64);
		ok();
	}

	printf("Basic adding: "); {
		is = intsetNew();
		is = intsetAdd(is,5,&success); assert(success);
		is = intsetAdd(is,6,&success); assert(success);
		is = intsetAdd(is,4,&success); assert(success);
		is = intsetAdd(is,4,&success); assert(!success);
		ok();
	}

	printf("Large number of random adds: "); {
		int inserts = 0;
		is = intsetNew();
		for (i = 0; i < 10; i++) {
			tmp = rand()%0x800;//0-2048
			//printf("%d\n",tmp);
			is = intsetAdd(is,tmp,&success);
			if (success) inserts++;
		}
		assert(intrev32ifbe(is->length) == inserts);
		checkConsistency(is);
		ok();
	}

	printf("Upgrade from int16 to int32: "); {
		is = intsetNew();
		is = intsetAdd(is,32,NULL);
		assert(intrev32ifbe(is->encoding) == INTSET_ENC_INT16);
		is = intsetAdd(is,65535,NULL);
		assert(intrev32ifbe(is->encoding) == INTSET_ENC_INT32);
		assert(intsetFind(is,32));
		assert(intsetFind(is,65535));
		checkConsistency(is);

		is = intsetNew();
		is = intsetAdd(is,32,NULL);
		assert(intrev32ifbe(is->encoding) == INTSET_ENC_INT16);
		is = intsetAdd(is,-65535,NULL);
		assert(intrev32ifbe(is->encoding) == INTSET_ENC_INT32);
		assert(intsetFind(is,32));
		assert(intsetFind(is,-65535));
		checkConsistency(is);
		ok();
	}

	return 0;
}

