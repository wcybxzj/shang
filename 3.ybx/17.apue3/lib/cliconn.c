#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define	CLI_PATH	"/var/tmp/"
#define	CLI_PERM	S_IRWXU			/* rwx for user only */

int cli_conn(const char *name){
	int fd, ret, err ,rval;
	struct sockaddr_un un, sun;
	int do_unlink = 0;

	if (strlen(name)>=sizeof(un.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd<0) {
		return -1;
	}

	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s/05%d", CLI_PATH, getpid());
	printf("client side socket file is :%s\n", un.sun_path);

	/*保证bind创建文件之前并不存在此socket文件*/
	unlink(un.sun_path);

	/*bind会创建要绑定的文件*/
	ret = bind(fd, (struct sockaddr *)&un, sizeof(un));
	if (ret < 0) {
		rval = -2;
		goto errout;
	}

	ret = chmod(un.sun_path, CLI_PERM);
	if (ret < 0) {
		rval = -3;
		do_unlink = 1;
		goto errout;
	}

	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, name);

	ret = connect(fd, (struct sockaddr_un*)&sun, sizeof(sun));
	if (ret<0) {
		rval = -4;
		do_unlink = 1;
		goto errout;
	}
	return fd;

errout:
	err = errno;
	close(fd);
	if (do_unlink) {
		unlink(un.sun_path);
	}
	errno = err;
	return rval;
}
