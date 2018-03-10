#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hiredis.h>

//如果要用同步阻塞来写SUBSCRIBE比较麻烦 还要循环，还是老实用异步API吧
int main(int argc, char **argv) {
    unsigned int j;
    redisContext *c;
    redisReply *r;
    const char *hostname = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }


	r = redisCommand(c,"SUBSCRIBE sub1 sub2");

	if (r->type == REDIS_REPLY_ARRAY) {
		for (j = 0; j < r->elements; j++) {
			if (r->element[j]->type==REDIS_REPLY_INTEGER) {
				printf("%u) %ld\n", j, r->element[j]->integer);
			}else{
				printf("%u) %s\n", j, r->element[j]->str);
			}
		}
	}


	freeReplyObject(r);





	/* Disconnects and frees the context */
	redisFree(c);

	return 0;
}
