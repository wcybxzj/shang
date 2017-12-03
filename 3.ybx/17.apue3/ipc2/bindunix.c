#include "apue.h"
#include <sys/socket.h>
#include <sys/un.h>

/*原版*/
void version1()
{
	int fd, size, ret;
	struct sockaddr_un un;
	un.sun_family = AF_UNIX;
	/*创建的UNIX套接字文件*/
	strcpy(un.sun_path, "foo.socket");
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd<0) {
		err_sys("socket failed");
	}

	/*原版写法*/
	/*
	size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	ret = bind(fd, (struct sockaddr *)&un, size);
	*/

	/*我的写法*/
	ret = bind(fd, (struct sockaddr *)&un, sizeof(struct sockaddr_un));

	if (ret<0) {
		err_sys("bind() error");
	}
	printf("unix domain socket bound success \n");

}


/*socket 创建有名UNIX域套接字,可用于无关系进程间通信*/
/*socketpair 创建无名UNIX域套接字,只能用于父子间进程间通信*/

/*
[root@web11 ipc2]# ./bindunix
unix domain socket bound success
[root@web11 ipc2]# ./bindunix
bind() error: Address already in use
[root@web11 ipc2]# rm foo.socket -f
[root@web11 ipc2]# ./bindunix
unix domain socket bound success
*/
int main(int argc, const char *argv[])
{
	version1();
	return 0;
}
