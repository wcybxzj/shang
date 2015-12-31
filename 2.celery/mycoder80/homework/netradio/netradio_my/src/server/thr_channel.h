#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__
#include "medialib.h"
int thr_channel_create(struct mlib_listentry_st *);
int thr_channel_destory(struct mlib_listentry_st *);
int thr_channel_destoryall(void);
#endif
