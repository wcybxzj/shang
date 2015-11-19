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
	off_t offset;	mytbf *tbf;
	mytbf *tbf;


int mlib_getchnlist(struct mlib_chn_st **list, int *listsize){
	struct mlib_chn_st *list_ptr;
	char pahtstr[PATHSIZE];
	glob_t list_res;

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

}
