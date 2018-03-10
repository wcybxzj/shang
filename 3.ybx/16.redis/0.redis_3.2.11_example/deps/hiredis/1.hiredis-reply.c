#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fmacros.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

#include <hiredis.h>
#include <net.h>
#include <sds.h>

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
		return REDIS_ERR;
	}
	return REDIS_OK;
}

int ybx_redisGetReplyFromReader(redisContext *c, void **reply) {
	if (redisReaderGetReply(c->reader, reply) == REDIS_ERR) {
		return REDIS_ERR;
	}
	return REDIS_OK;
}

int ybx_redisGetReply(redisContext *c, void **reply) {
	//int wdone = 0;
	void *aux = NULL;

	if (ybx_redisBufferRead(c) == REDIS_ERR)
		return REDIS_ERR;

	if (ybx_redisGetReplyFromReader(c,&aux) == REDIS_ERR)
		return REDIS_ERR;

	/* Set reply object */
	if (reply != NULL) *reply = aux;
	return REDIS_OK;
}


void *get_reply(void){
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
}


void print_reply(redisReply* reply)
{
	unsigned int i;
	int type = reply->type;
	if (type == REDIS_REPLY_ARRAY) {
		for (i = 0; i < reply->elements; i++) {
			print_reply(reply->element[i]);
		}
	}else if(type == REDIS_REPLY_INTEGER){
		printf("%lld\n", reply->integer);
	}else{
		printf("%s\n", reply->str);
	}
}


//研究hiredis对服务端的响应进行解析,模拟网站中的2层树结构
//http://blog.csdn.net/gqtcgq/article/details/51344232
int main(int argc, const char *argv[])
{
	redisReply *reply;
	reply = get_reply();
	if (reply==NULL) {
		return 0;
	}

	print_reply(reply);
	return 0;
}
