#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <proto.h>

#include "medialib.h"
#include "mytbf.h"

#define PATH	"/home/brian/coder80/proj/list"
#define PATHSIZE	1024
#define BUFSIZE		1024
#define CPS			10
#define BURST		1000


struct mytbf_st
{
	int cps;
	int burst;
	int token;
	int pos;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};

struct mlib_chn_context_st
{
	chnid_t chnid;
    char *desc;
	glob_t globres;
	int cur;
	int fd;
	off_t offset;
	mytbf_t *tbf;
};

static struct mlib_chn_context_st *cont_ptr;

static int getchncontext(const char *path, \
						 struct mlib_chn_context_st **ptr, \
						 const int num, \
						 chnid_t chnid)
{
	char pathstr[PATHSIZE];
	char desc_buf[BUFSIZE];
	FILE *fp;
	int err, nr = num-1;
	struct mlib_chn_context_st *me = *ptr;

	me = realloc(me,sizeof(struct mlib_chn_context_st)*num);
	if(NULL == me)
		return -1;

	strncpy(pathstr, path, PATHSIZE);
	strncat(pathstr, "/desc.txt", PATHSIZE);
	fp = fopen(pathstr, "r");
	if(NULL == fp) {
		perror("fopen()");
		free(me);
		return -1;
	}
	fgets(desc_buf, BUFSIZE, fp);
	me[nr].chnid = chnid;
	me[nr].desc = strdup(desc_buf);
	fclose(fp);

	strncpy(pathstr, path, PATHSIZE);
	strncat(pathstr, "/*.mp3", PATHSIZE);
	err = glob(pathstr, 0, NULL, &me[nr].globres);
	if(err < 0) {
		free(me);
		return -1;	
	}
	me[nr].cur = 0;
	me[nr].fd = open(me[nr].globres.gl_pathv[me[nr].cur]);
	if(me[nr].fd < 0) {
		free(me);
		return -1;
	}
	me[nr].offset = 0;
	me[nr].tbf = mytbf_init(CPS, BURST);
	*ptr = me;

	return 0;
}

int mlib_getchnlist(struct mlib_chn_st **list,int *listsize)
{
	struct mlib_chn_st *list_ptr;
	struct mlib_chn_context_st *ptr = NULL;
	glob_t list_res;
	int err, i, chnum;
	char pathstr[PATHSIZE];
	chnid_t chnid = MINCHNID;
	
	strncpy(pathstr, PATH, PATHSIZE);
	strncat(pathstr, "/*", PATHSIZE);
	err = glob(pathstr, 0, NULL, &list_res);
	if(err) {
		fprintf(stderr, "glob():%s\n", strerror(err));
		return -1;
	}
	if(list_res.gl_pathc > MAXCHNID)
		chnum = MAXCHNID;
	else 
		chnum = list_res.gl_pathc;

	list_ptr = malloc(sizeof(struct mlib_chn_st)*chnum);
	if(NULL == list_ptr)
		return -1;

	for(i = 0; i < chnum; i++) {
		if(getchncontext(list_res.gl_pathv[i], \
					&ptr, i+1, chnid) < 0)
			return -1;
		list_ptr[i].chnid = ptr[i].chnid;
		list_ptr[i].desc = strdup(ptr[i].desc);
		chnid++;
	}

	*list = list_ptr;
	*listsize = chnum;
	cont_ptr = ptr;
		
	return 0;
}

int mlib_freechnlist(struct mlib_chn_st *list)
{
	free(list);
	free(cont_ptr);

	return 0;
}

int mlib_getchndata(chnid_t chnid, char *buf, int size)
{
	int id = chnid - 1;
	int tsize, len;

	tsize = mytbf_fetchtoken(cont_ptr[id].tbf, size);
	if(tsize < 0)
		return -1;
	len = read(cont_ptr[id].fd, buf, tsize);
	if(len < 0)
		return -1;
	else if(!len) {
		cont_ptr[id].cur++;
		if(cont_ptr[id].cur	== cont_ptr[id].globres.gl_pathc)
			cont_ptr[id].cur = 0;
		cont_ptr[id].fd = \
		open(cont_ptr[id].globres.gl_pathv[cont_ptr[id].cur]);
		if(cont_ptr[id].fd < 0)
			return -1;
	}else {
		mytbf_returntoken(cont_ptr[id].tbf, tsize-len);
	}

	return len;
}


