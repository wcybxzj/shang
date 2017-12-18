
#ifndef __REDIS_ASSERT_H__
#define __REDIS_ASSERT_H__

#include <unistd.h> /* for _exit() */

//临时先用原来的
#include <assert.h>

//#define assert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
//
//void _redisAssert(char *estr, char *file, int line);

#endif
