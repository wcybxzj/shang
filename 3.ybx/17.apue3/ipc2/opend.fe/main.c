#include "opend.h"

char errmsg[];
int oflag;
char* pathname;

int main(int argc, const char *argv[])
{
	/*
	 * 调用此进程的open.fe/openclient
	 * 将本进程的标准输入输出都指向socketpair fd[1]
	 */
	int nread;
	char buf[MAXLINE];
	while (1) {
		nread = read(STDIN_FILENO, buf, MAXLINE);
		if (nread<0) {
			err_sys("read error");
		}else if(nread==0){
			break;
		}else{
			handle_request(buf, nread, STDOUT_FILENO);
		}
	}

	return 0;
}
