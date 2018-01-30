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

//quicklist 部分
#define yell(str, ...) printf("ERROR! " str "\n\n", __VA_ARGS__)

#define OK printf("\tOK\n")

#define ERROR                                                                  \
    do {                                                                       \
        printf("\tERROR!\n");                                                  \
        err++;                                                                 \
    } while (0)

#define ERR(x, ...)                                                            \
    do {                                                                       \
        printf("%s:%s:%d:\t", __FILE__, __FUNCTION__, __LINE__);               \
        printf("ERROR! " x "\n", __VA_ARGS__);                                 \
        err++;                                                                 \
    } while (0)

#define TEST(name) printf("test — %s\n", name);
#define TEST_DESC(name, ...) printf("test — " name "\n", __VA_ARGS__);

const size_t optimization_level[] = {4096, 8192, 16384, 32768, 65536};

static void ql_info(quicklist *ql) {
	//printf("\n===========================\n");
    //printf("Container length: %lu\n", ql->len);
    //printf("Container size: %lu\n", ql->count);
    //if (ql->head)
    //    printf("\t(zsize head: %d)\n", ziplistLen(ql->head->zl));
    //if (ql->tail)
    //    printf("\t(zsize tail: %d)\n", ziplistLen(ql->tail->zl));
    //printf("\n");
}


/* Iterate over an entire quicklist.
 * Print the list if 'print' == 1.
 *
 * Returns physical count of elements found by iterating over the list. */
static int _itrprintr(quicklist *ql, int print, int forward) {
    quicklistIter *iter =
        quicklistGetIterator(ql, forward ? AL_START_HEAD : AL_START_TAIL);
    quicklistEntry entry;
    int i = 0;
    int p = 0;
    quicklistNode *prev = NULL;
    while (quicklistNext(iter, &entry)) {
        if (entry.node != prev) {
            /* Count the number of list nodes too */
            p++;
            prev = entry.node;
        }
        if (print) {
            printf("[%3d (%2d)]: [%.*s] (%lld)\n", i, p, entry.sz,
                   (char *)entry.value, entry.longval);
        }
        i++;
    }
    quicklistReleaseIterator(iter);
    return i;
}
static int itrprintr(quicklist *ql, int print) {
    return _itrprintr(ql, print, 1);
}

static int itrprintr_rev(quicklist *ql, int print) {
    return _itrprintr(ql, print, 0);
}

#define ql_verify(a, b, c, d, e)                                               \
    do {                                                                       \
        err += _ql_verify((a), (b), (c), (d), (e));                            \
    } while (0)

/* Verify list metadata matches physical list contents. */
static int _ql_verify(quicklist *ql, uint32_t len, uint32_t count,
                      uint32_t head_count, uint32_t tail_count) {
    int errors = 0;

    ql_info(ql);
    if (len != ql->len) {
        yell("quicklist length wrong: expected %d, got %u", len, ql->len);
        errors++;
    }

    if (count != ql->count) {
        yell("quicklist count wrong: expected %d, got %lu", count, ql->count);
        errors++;
    }

	int loopr = itrprintr(ql, 0);
    if (loopr != (int)ql->count) {
        yell("quicklist cached count not match actual count: expected %lu, got "
             "%d",
             ql->count, loopr);
        errors++;
    }

    int rloopr = itrprintr_rev(ql, 0);
    if (loopr != rloopr) {
        yell("quicklist has different forward count than reverse count!  "
             "Forward count is %d, reverse count is %d.",
             loopr, rloopr);
        errors++;
    }

    if (ql->len == 0 && !errors) {
        OK;
        return errors;
    }

    if (ql->head && head_count != ql->head->count &&
        head_count != ziplistLen(ql->head->zl)) {
        yell("quicklist head count wrong: expected %d, "
             "got cached %d vs. actual %d",
             head_count, ql->head->count, ziplistLen(ql->head->zl));
        errors++;
    }

    if (ql->tail && tail_count != ql->tail->count &&
        tail_count != ziplistLen(ql->tail->zl)) {
        yell("quicklist tail count wrong: expected %d, "
             "got cached %u vs. actual %d",
             tail_count, ql->tail->count, ziplistLen(ql->tail->zl));
        errors++;
    }


    if (!errors)
        OK;
    return errors;
}



int quicklistTest() {
	int optimize_start = -(int)(sizeof(optimization_level) / sizeof(*optimization_level));
	printf("Starting optimization offset at: %d\n", optimize_start);//-5

	int options[] = {0, 1, 2, 3, 4, 5, 6, 10};
	size_t option_count = sizeof(options) / sizeof(*options);//8

	for (int _i = 0; _i < (int)option_count; _i++) {

		//printf("ziplist depth is:%d\n", options[_i]);
		//printf("add to head of empty list");
		//{
		//	quicklist *ql = quicklistNew(-2, options[_i]);
		//	quicklistPushHead(ql, "hello", strlen("hello"));
		//	ql_verify(ql, 1, 1, 1, 1);
		//	quicklistRelease(ql);
		//}

		for (int f = optimize_start; f < 32; f++) {
			TEST_DESC("add to head 5x at fill %d at compress %d", f, options[_i])
			 {
				quicklist *ql = quicklistNew(f, options[_i]);
				for (int i = 0; i < 5; i++)
					quicklistPushHead(ql, genstr("hello", i), 32);
				if (ql->count != 5)
					ERROR;
				if (f == 32)
					ql_verify(ql, 1, 5, 5, 5);
				quicklistRelease(ql);
			}
		}
	}
}

void test1()
{
	int print=1;
	int forward=1;
	int ret;
	int fill =-2;//-2 每个节点的ziplist字节大小不能超过8kb
	int compress = 0;//不压缩
	quicklist *ql = quicklistNew(-2,0);
	ret = quicklistPushHead(ql, "hello", strlen("hello"));
	//printf("%d\n",ret);//1
	ret = quicklistPushHead(ql, "nice", strlen("nice"));
	//printf("%d\n",ret);//0

	//打印数据信息
	_itrprintr(ql,print, forward);

	unsigned char *str;
	unsigned int str_size;
	long long number;
	ret = quicklistPop(ql, QUICKLIST_HEAD, &str, &str_size, &number);
	if (ret) {
		if (str) {
			printf("%s\n",str);
		}else{
			printf("ld\n",number);
		}
	}
}

void test2()
{
	int fill =2;//一个quicklistNode节点最多存3个ziplist entry
	int compress = 1;//1 表示quicklist两端各有一个quicklist节点不压缩，中间的quicklist节点压缩
	quicklist *ql = quicklistNew(fill,compress);
	int i, ret;
	char *tmp_str;
	int len;

	char *name[8];
	for (i = 0; i < 8; i++) {
		name[i]=calloc(100,sizeof(char));
	}

	memset(name[0],'a',99);
	memset(name[1],'b',99);
	memset(name[2],'c',99);
	memset(name[3],'d',99);
	memset(name[4],'e',99);
	memset(name[5],'f',99);
	memset(name[6],'g',99);
	memset(name[7],'h',99);

	for (i = 0; i < 8; i++) {
		len = strlen(name[i]);
		ret = quicklistPushHead(ql,name[i],len);
		if (ret) {
			printf("创建新的 quicklist node\n");
		}else{
			printf("使用老的 quicklist node\n");
		}
	}

}

int main(int argc, const char *argv[])
{

	initServerConfig();
	createSharedObjects();

	//test1();
	test2();
	//quicklistTest();
	return 0;
}

