#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define QLEN	10

int  serv_listen(const char *name)
{
	int fd, err, rval, ret;
	struct sockaddr_un un;
	if (strlen(name)>=sizeof(un.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		return -2;
	}

	/*防止要创建的 服务端socket文件已经存在*/
	unlink(name);

	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);

	ret = bind(fd, (struct sockaddr *)&un, sizeof(un));
	if (ret < 0) {
		rval = -3;
		goto errout;
	}

	ret = listen(fd, QLEN);
	if (ret < 0) {
		rval = -4;
		goto errout;
	}
	return fd;

errout:
	err = errno;
	close(fd);
	errno = err;
	return rval;
}
