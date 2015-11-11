#include <stdio.h>
#include <stdlib.h>
#include "meidalib.h"

struct channel_context_st{
	chnid_t chnid;
	char *desc;
	glob_t mp3glob;
	int pos;//当前播放的下标
	int fd;//mp3的fd
	off_t offset;//播放的位置
	mytbf_t *tbf;
};

int mlib_getchnlist(struct mlib_listentry_st **, int *){

}

int mlib_freechnlist(struct mlib_listentry_st *){

}

//返回值为实际读到的数量
ssize_t mlib_readchn(chnid_t, void *, size_t){

}
