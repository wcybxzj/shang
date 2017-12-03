#include "opend.h"
#include <fcntl.h>

void handle_request(char *buf, int nread, int fd)
{
	int ret;
	/*必须NULL结尾*/
	if (buf[nread-1] != 0) {
		snprintf(errmsg, MAXLINE,
				"buf must end with NULL:%*.*s",nread, nread, buf);
		send_err(fd, -1, errmsg);
		return;
	}

	/*解析buf并且从中内容提取*/
	ret = buf_args(buf, cli_args);
	if (ret<0) {
		send_err(fd, -1, errmsg);
		return;
	}

	/*open*/
	int newfd = open(pathname, oflag);
	if (newfd < 0) {

		snprintf(errmsg, MAXLINE, "can not open:%s oflag:%d, errstr:%s\n",
				pathname, oflag,strerror(errno));
		send_err(fd, -1, errmsg);
		return;
	}

	/*sendmsg*/
	ret = send_fd(fd, newfd);
	if (ret<0) {
		err_sys("send_fd error");
	}
	close(newfd);
}
