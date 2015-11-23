#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <proto.h>
#include "medialib.h"
#include "server.h"

static void *thr_channel(void *ptr){
	int len;
	char buf[BUFSIZE];
	struct msg_channel_st *sbuf;
	chnid_t cid = (chnid_t) ptr;

	while (1) {
		len = mlib_getchndata(cid, buf, BUFSIZE);
		if (len > 0) {
			sbuf = malloc(len+1);
			sbuf->chnid = cid;
			memcpy(sbuf->data, buf, len);
			if (sendto(sd, sbuf, len+1, 0, (void *)&raddr, sizeof(raddr)) < 0) {
				break;
			}
			free(sbuf);
			sbuf = NULL;
		}else if(len < 0){
			break;
		}
	}

	pthread_exit(NULL);
}

int thr_channel_create(pthread_t* tid, chnid_t cid){
	if (pthread_create(tid, NULL, thr_channel, (void *)cid)) {
		return -1;
	}
	return 0;
}

int thr_channel_destroy(chnid_t chnid){
	pthread_cancel(tid[chnid]);
	pthread_join(tid[chnid] ,NULL);
	return 0;
}

int thr_channel_destroyall(int listsize){
	int i;
	for (i = 1; i <=listsize; i++) {
		thr_channel_destroy(i);
	}
	return 0;
}
