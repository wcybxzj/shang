#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <errno.h>

#define	STALE	30/* 要求客户端socket是最近30秒内的新文件*/

/*uid_t 无符号32位整形*/

/*
man 7 unix
struct sockaddr_un {
	sa_family_t sun_family;
	char        sun_path[UNIX_PATH_MAX];
};
*/
int  serv_accept(int listenfd, uid_t *uidptr)
{
	int ret, err, rval;
	int clifd;
	char *name;
	socklen_t len;
	struct sockaddr_un un;
	struct stat statbuf;
	time_t staletime;

	name = malloc(sizeof(un.sun_path));
	if (name == NULL) {
		return -1;
	}
	memset(name, 0, sizeof(name));

	clifd = accept(listenfd, (struct sockaddr *)&un, &len);
	if (clifd < 0) {
		free(name);
		return -2;
	}

	len -=offsetof(struct sockaddr_un, sun_path);
	memcpy(name, un.sun_path, len);

	ret = stat(name, &statbuf);
	if (ret<0) {
		rval = -3;
		goto errout;
	}

	/*file is socket?*/
	if (S_ISSOCK(statbuf.st_mode)==0) {
		rval = -4;
		goto errout;
	}

	/*file permisson is 700*/
	if ( (statbuf.st_mode & (S_IRWXG|S_IRWXO)) ||
		(statbuf.st_mode & S_IRWXU) != S_IRWXU ) {
		rval =-5;
		goto errout;
	}

	/*file time is recent 30 seconds*/
	staletime = time(NULL) - STALE;
	if (statbuf.st_atime < staletime ||
		statbuf.st_ctime < staletime ||
		statbuf.st_mtime < staletime) {
		rval = -6;
		goto errout;
	}

	/*uidptr*/
	if (uidptr != NULL) {
		*uidptr = statbuf.st_uid;
	}

	/*accept后就可以删除客户端socket文件*/
	unlink(name);
	free(name);
	return clifd;

errout:
	err = errno;
	close(clifd);
	free(name);
	errno = err;
	return rval;

}
