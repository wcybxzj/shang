#include "apue.h"

int main(int argc, const char *argv[])
{
	struct stat			statbuf;
	if (stat("foo.socket", &statbuf) < 0) {
		err_sys("stat() error");
	}

#ifdef	S_ISSOCK	/* not defined for SVR4 */
	if (S_ISSOCK(statbuf.st_mode) == 0) {
		printf("file is not a socket file\n");
	}else{
		printf("file is a socket file\n");
	}
#endif

	/*文件权限要求700*/
	if (statbuf.st_mode & (S_IRWXG | S_IRWXO)){
		printf("错误 group 和 other 要求完全没权限\n");
	}else{
		printf("正确 group 和 other 完全没权限\n");
	}
	if((statbuf.st_mode & S_IRWXU) != S_IRWXU) {
		printf("错误 user必须是全权限\n");
	}else{
		printf("正确 user是全权限\n");
	}


	return 0;
}
