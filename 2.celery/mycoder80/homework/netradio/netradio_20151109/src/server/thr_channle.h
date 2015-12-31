#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__


int thr_channel_create(pthread_t *, chnid_t);
void thr_channel_destroy(chnid_t);
void thr_channel_destroyall(int);

#endif

