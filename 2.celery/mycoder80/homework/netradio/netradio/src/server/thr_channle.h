#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__

int thr_channel_create(pthread_t *tid,chnid_t cid);
int thr_channel_destroy(chnid_t chnid);
int thr_channel_destroyall(int listsize);

#endif

