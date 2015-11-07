#ifndef MEDIALIB_H__
#define MEDIALIB_H__

struct mlib_chn_st
{
	chnid_t chnid;
	char *desc;
};


int mlib_getchnlist(struct mlib_chn_st **,int *);
int mlib_freechnlist(struct mlib_chn_st *);


#endif


