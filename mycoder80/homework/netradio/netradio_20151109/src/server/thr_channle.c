#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <proto.h>

#include "thr_channle.h"
#include "medialib.h"
#include "server.h"


static void *thr_channel(void *ptr)
{
	char buf[BUFSIZE];
	chnid_t chnid = (chnid_t)ptr;
	int len, size;
	struct msg_chnnal_st *sbuf;

	while(1) {
		len = mlib_getchndata(chnid, buf, BUFSIZE);
		if(len < 0)
			break;
		else if(len > 0) {
			size = len + 1;
			sbuf = malloc(size);
			if(NULL == sbuf)
				break;
			sbuf->chnid = chnid;
			memcpy(sbuf->data, buf, len);
			if(sendto(sd, sbuf, size, 0, (void *)&raddr, sizeof(raddr)) < 0) 
				break;
			free(sbuf);
			sbuf = NULL;
		}
	}

	pthread_exit(NULL);
}

int thr_channel_create(pthread_t *tid, chnid_t chnid)
{	
	if(pthread_create(tid, NULL, thr_channel, (void *)chnid))
		return -1;

	return 0;
}

void thr_channel_destroy(chnid_t chnid)
{
	pthread_join(tid[chnid], NULL);	
}
void thr_channel_destroyall(int listsize)
{
	chnid_t i;

	for(i = MINCHNID; i <= listsize; i++)
		thr_channel_destroy(i);
}






