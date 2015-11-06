#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__

#include<proto.h>

int thr_channel_create(chnid_t *chnid);
int thr_channel_destroy(chnid_t chnid);
int thr_channel_destroyall(int listsize);

#endif

