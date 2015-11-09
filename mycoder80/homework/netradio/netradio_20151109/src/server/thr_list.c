#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <proto.h>

#include "medialib.h"
#include "thr_list.h"
#include "server.h"


static struct msg_list_st *sbuf;
static int lsize;

static void *thr_list(void *ptr)
{
	while(1) {
		if(sendto(sd, sbuf, lsize, 0, (void *)&raddr, sizeof(raddr)) < 0) {
			perror("sendto()");
			break;
		}
		sleep(1);
	}

	pthread_exit(NULL);
}

int thr_list_create(struct mlib_chn_st *ptr, int size)
{
	int i, err;
	char *pos;

	for(i = 0; i < size; i++) {
		lsize += strlen(ptr[i].desc) + 2;
	}
	lsize++;
	sbuf = malloc(lsize);
	if(NULL == sbuf) {
		fprintf(stderr, "malloc() is error\n");
		return -1;
	}

	sbuf->chnid = CHNID_LIST;
	pos = sbuf->entry;
	for(i = MINCHNID; i <= size; i++) {
		*pos = i;
		pos++;
		strcpy(pos, ptr[i-1].desc);
		pos += strlen(pos) + 1;
	}

	err = pthread_create(tid, NULL, thr_list, NULL);
	if(err) {
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		return -1;
	}

	return 0;
}

void thr_list_destroy(void)
{
	pthread_join(tid[0], NULL);
	free(sbuf);
	sbuf = NULL;
}


