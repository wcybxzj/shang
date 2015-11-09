#ifndef MEDIALIB_H__
#define MEDIALIB_H__

#include <proto.h>

#define DEFAULT_PATH		"/root/Desktop/coder80/proj/list"
#define PATHSIZE	1024
#define BUFSIZE		1024
#define CPS			(16*1024)
#define BURST		(240*1024)


struct mlib_chn_st {
	chnid_t chnid;
	char *desc;
};


int mlib_getchnlist(struct mlib_chn_st **,int *);
int mlib_freechnlist(struct mlib_chn_st **, int);
int mlib_getchndata(chnid_t, char *, int);

#endif


