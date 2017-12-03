#include "apue.h"
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

void daemonize(const char *cmd)
{
	int i;
	pid_t pid;
	int ret;
	struct rlimit rl;

	/*
	放开文件权限防止以前设置了umask,
	造成后边创建文件权限受影响
	*/
	umask(0);

	ret = getrlimit(RLIMIT_NOFILE, &rl);
	if (ret < 0) {
		err_quit("%s: can not get resource limit", cmd);
	}

	pid = fork();
	if (pid<0) {
		err_quit("%s:can not fork", cmd);
	}else if (pid>0){
		exit(1);
	}else{
		setsid();
	}
	
	ret = chdir("/");
	if (ret<0) {
		err_quit("%s: can not fork", cmd);
	}

	for (i = 0; i < rl.rlim_max; i++) {
		close(i);
	}
	int fd0, fd1, fd2;
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
	/*
	 LOG_CONS:
	 如果信息不能被记录到日志文件中，就把它们发送到控制台
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0!=0 || fd1!=1 || fd2!=2) {
		syslog(LOG_ERR, "unexpected file desciptors %d  %d %d",
				fd0, fd1, fd2);
		exit(-1);
	}
}
