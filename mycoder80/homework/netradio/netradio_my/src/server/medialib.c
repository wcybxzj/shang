#include <stdio.h>
#include <stdlib.h>
#include "meidalib.h"

#define PATHSIZE 1024
struct channel_context_st{
	chnid_t chnid;
	char *desc;
	glob_t mp3glob;
	int pos;//当前播放的下标
	int fd;//mp3的fd
	off_t offset;//播放的位置
	mytbf_t *tbf;
};

static struct chanel_cotext_st channel[MAXCHNID+1];//0 给节目单　1-100给channel

int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum){
	int i;
	int num = 0;
	char path[PATHSIZE];
	glob_t globres;

	struct mlib_listetry *ptr;
	struct channel_context_st *res;

	for (i = 0; i <MAXCHNID+1; i++) {
		channel[i].chnid = -1;
	}

	//debug
	snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);

	if (glob(path, 0, NULL, globres)) {
		return -1;
	}

	ptr = malloc(sizeof(struct milb_listetry_st) * globres.gl_pathc);
	if (NULL == ptr) {
		syslog(LOG_ERR, "malloc error");
		exit(0);
	}

	for (i = 0; i < globres.gl_pathc; i++) {
		//globres.pathv[i]－-> "/home/ybx/media/ch1"
		path2entry(globres.gl_pathv[i]);
		num++;
	}

	*result = 

	*resnum = num;

	return 0;
}

int mlib_freechnlist(struct mlib_listentry_st *){

}

//返回值为实际读到的数量
ssize_t mlib_readchn(chnid_t, void *, size_t){

}
