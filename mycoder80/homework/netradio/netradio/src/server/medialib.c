#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <glob.h>
#include <proto.h>

#include "medialib.h"
#include "mytbf.h"
#include "server.h"

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

static int getchncontent(const char *path,\
						struct mlib_chn_context_st **ptr,\
						const int num,\
						chnid_t chnid)
{
	int fd;
	FILE *fp;
	int err;
	char pathstr[PATHSIZE];
	char str[1024];
	int ptr_num = num-1;

	//realloc
	(*ptr) = realloc((*ptr), sizeof(struct mlib_chn_context_st)*num);
	if (NULL == (*ptr)) {
		return -100;
	}

	//desc.txt
	strncpy(pathstr, path, PATHSIZE);
	strncat(pathstr, "/desc.txt", PATHSIZE);
	fp = fopen(pathstr,"r");
	fgets(str, 1024, fp);

	//*.mp3
	strncpy(pathstr, path, PATHSIZE);
	strncat(pathstr, "/*.mp3", PATHSIZE);
	err = glob(pathstr, 0, NULL, &((*ptr)[ptr_num].globres));
	if (err < 0) {
		return -1;
	}
	(*ptr)[ptr_num].cur = 0;
	fd = open((*ptr)[ptr_num].globres.gl_pathv[0], O_RDONLY);
	if (fd < 0) {
		return -2;
	}

	(*ptr)[ptr_num].chnid = chnid;
    (*ptr)[ptr_num].desc = strdup(str);
	(*ptr)[ptr_num].fd = fd;
	(*ptr)[ptr_num].offset = 0;
	(*ptr)[ptr_num].tbf = mytbf_init(CPS, BURST);

	return 0;
}

int mlib_getchnlist(struct mlib_chn_st **list, int *listsize){
	struct mlib_chn_st *list_ptr;
	struct mlib_chn_context_st *ptr = NULL;
	char pathstr[PATHSIZE];
	glob_t list_res;
	int i,err,chnum;
	chnid_t chnid = MINCHNID;

	strncpy(pathstr, server_conf.medpath, PATHSIZE);
	strncat(pathstr, "/*", PATHSIZE);
	err = glob(pathstr, 0, NULL, &list_res);
	if (err) {
		fprintf(stderr, "glob():%s\n", strerror(err));
		return -1;
	}

	if (list_res.gl_pathc > MAXCHNID) {
		chnum = MAXCHNID;
	}else{
		chnum = list_res.gl_pathc;
	}

	list_ptr = malloc(sizeof(struct mlib_chn_st)*chnum);
	if (NULL == list_ptr) {
		return -2;
	}

	for (i = 0; i < chnum; i++) {
		if (getchncontent(list_res.gl_pathv[i], &ptr, i+1, chnid)) {
			return -3;
		}
		list_ptr[i].chnid = ptr[i].chnid;
		list_ptr[i].desc= strdup(ptr[i].desc);
		chnid++;
	}

	*listsize = chnum;
	*list = list_ptr;
	cont_ptr = ptr;
	return 0;
}

int mlib_getchndata(chnid_t cid, char *buf,int size){
	int tsize;
	int cur;
	int len;
	int cur_cid = cid-1;
	tsize = mytbf_fetchtoken(cont_ptr[cur_cid].tbf, size);
	if (tsize < 0) {
		return -100;
	}
	while (1) {
		len = read(cont_ptr[cur_cid].fd , buf, tsize);
		//printf("len %d\n", len);
		if (len < 0) {
			return -1;
		}else if (!len) {
			close(cont_ptr[cur_cid].fd);
			cont_ptr[cur_cid].cur++;
			if (cont_ptr[cur_cid].cur == cont_ptr[cur_cid].globres.gl_pathc) {
				cont_ptr[cur_cid].cur = 0;
			}
			cur = cont_ptr[cur_cid].cur; 
			cont_ptr[cur_cid].fd = open(cont_ptr[cur_cid].globres.gl_pathv[cur],O_RDONLY);
			if (cont_ptr[cur_cid].fd < 0) {
				return -2;
			}
		}else {
			mytbf_returntoken(cont_ptr[cur_cid].tbf, tsize - len);
			break;
		}
	}
	return len;
}

