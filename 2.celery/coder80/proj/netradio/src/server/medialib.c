#ifndef MEDIALIB_H__
#define MEDIALIB_H__

struct mlib_chn_context_st
{
	chnid_t chnid;
    char *desc;
	glob_t globres;
	int cur;
	int fd;
	off_t offset;
	mytbf *tbf;

	.....	

};



#endif




