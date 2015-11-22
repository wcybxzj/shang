#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <proto.h>

#include "medialib.h"
#include "server.h"
#include "thr_list.h"

static struct msg_list_st *sbuf;
static int lsize;

static void *thr_list(void *ptr)
{
	while (1) {
		if (sendto(sd, sbuf, lsize, 0, (void*) &raddr, sizeof(raddr))<0) {
			perror("thr_list() err");
			break;
		}
		sleep(1);
	}
	pthread_exit(NULL);
}

int thr_list_create(struct mlib_chn_st *listptr, int listsize){
	int i, err;
	char *pos;

	//malloc
	for (i = 0; i < listsize; i++) {
		lsize+=strlen(listptr[i].desc)+2;
	}
	lsize++;
	sbuf = malloc(lsize);
	if (NULL == sbuf) {
		return -1;
	}

	sbuf->chnid = CHNID_LIST;
	pos = sbuf->entry;
	for (i = MINCHNID; i <= listsize; i++) {
		*pos = i;
		pos++;
		strcpy(pos, listptr[i-1].desc);
		pos+= strlen(pos)+1;
	}

	err = pthread_create(tid, NULL, thr_list, NULL);
	if (err) {
		return -2;
	}
	return 0;
}
