#include "fmacros.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include "hiredis.h"
#include "net.h"
#include "sds.h"

int ybx_redisReaderFeed(redisReader *r, const char *buf, size_t len) {
    sds newbuf;

    if (r->err)
        return REDIS_ERR;

    if (buf != NULL && len >= 1) {
        if (r->len == 0 && r->maxbuf != 0 && sdsavail(r->buf) > r->maxbuf) {
            sdsfree(r->buf);
            r->buf = sdsempty();
            r->pos = 0;
            assert(r->buf != NULL);
        }

        newbuf = sdscatlen(r->buf,buf,len);
        if (newbuf == NULL) {
            __redisReaderSetErrorOOM(r);
            return REDIS_ERR;
        }

        r->buf = newbuf;
        r->len = sdslen(r->buf);
    }

    return REDIS_OK;
}

int ybx_redisBufferRead(redisContext *c) {
	char buf[1024*16] = "*3\r\n*3\r\n:11\r\n:12\r\n:13\r\n*3\r\n:21\r\n:22\r\n:23\r\n:31\r\n";
	int nread = strlen(buf);
	
	if (ybx_redisReaderFeed(c->reader,buf,nread) != REDIS_OK) {
		__redisSetError(c,c->reader->err,c->reader->errstr);
		return REDIS_ERR;
	}
	return REDIS_OK;
}

int ybx_redisGetReplyFromReader(redisContext *c, void **reply) {
	if (ybx_redisReaderGetReply(c->reader,reply) == REDIS_ERR) {
		__redisSetError(c,c->reader->err,c->reader->errstr);
		return REDIS_ERR;
	}
	return REDIS_OK;
}

int ybx_redisGetReply(redisContext *c, void **reply) {
	int wdone = 0;
	void *aux = NULL;

	if (ybx_redisBufferRead(c) == REDIS_ERR)
		return REDIS_ERR;

	if (ybx_redisGetReplyFromReader(c,&aux) == REDIS_ERR)
		return REDIS_ERR;

	/* Set reply object */
	if (reply != NULL) *reply = aux;
	return REDIS_OK;
}



int main(int argc, const char *argv[])
{
	//模拟数据,伪装成建立了连接
    redisContext *c;
    c = redisContextInit();
    if (c == NULL)
        return NULL;
    c->flags |= REDIS_BLOCK;
	c->fd = 123;//哑数据
	c->flags |= REDIS_CONNECTED;

    void *reply;
    if (c->flags & REDIS_BLOCK) {
        if (ybx_redisGetReply(c,&reply) != REDIS_OK){
            return NULL;
		}else{
			return reply;
		}
	}
	return NULL;

	return 0;
}
