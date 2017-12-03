#include "open.h"
#include <assert.h>
#include <fcntl.h>
#include <sys/uio.h>

int
csopen(char *name, int oflag){
	pid_t pid;
	int ret, len;
	static int fd[2]={-1, -1};
	char buf[20];
	struct iovec iov[3];

	/*
	只有第一个调用csopen 才会创建socketpair+fork+exec
	第二次调用csopen,父进程直接请求服务就行
	*/
	if (fd[0]==-1 && fd[1]==-1) {
		/*创建socketpair*/
		ret = fd_pipe(fd);
		if (ret ==-1) {
			err_ret("fd_pipe()");
			return -1;
		}

		pid = fork();
		assert(pid>=0);
		if (pid==0) {/*child*/
			close(fd[0]);
			ret = dup2(fd[1], STDIN_FILENO);
			if (ret != STDIN_FILENO) {
				err_ret("dup()");
				return -1;
			}
			ret = dup2(fd[1], STDOUT_FILENO);
			if (ret != STDOUT_FILENO) {
				err_ret("dup()");
				return -1;
			}
			ret = execl("../opend.fe/opend", "opend", NULL);
			if (ret==-1) {
				err_ret("execl() error");
				return -1;
			}
		}
		/*parent*/
		close(fd[1]);
	}

	sprintf(buf, " %d", O_RDONLY);
	iov[0].iov_base = CL_OPEN " ";
	iov[0].iov_len  = strlen(CL_OPEN)+1;
	iov[1].iov_base = name;
	iov[1].iov_len = strlen(name);
	iov[2].iov_base = buf;
	iov[2].iov_len = strlen(buf)+1;
	len = iov[0].iov_len+iov[1].iov_len+iov[2].iov_len;
	if (writev(fd[0], &iov[0], 3)!=len) {
		err_ret("writev()");
		return -1;
	}

	return recv_fd(fd[0], write);
}
