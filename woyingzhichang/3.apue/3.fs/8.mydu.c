#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define SIZE  1024

//防止死循环递归
//因为文件系统不是典型的树状结构而是带回路的.和..
static int not_dot(const char *pathname)
{
	char *tmp;
	tmp = strrchr(pathname, '/');
	if (strcmp(tmp+1, ".") == 0||strcmp(tmp+1, "..") == 0) {
		return 0;
	}
	return 1;
}

static int64_t mydu(const char *pathname)
{
	int64_t blocks = 0;
	int i;
	glob_t res;
	struct stat sb;
	char pattern[SIZE];

	if (lstat(pathname, &sb) < 0) {
		perror("lstat():");
		exit(1);
	}

	if (!S_ISDIR(sb.st_mode)) {
		return sb.st_blocks;
	}

	strncpy(pattern, pathname, SIZE);
	strncat(pattern, "/*", SIZE);
	glob(pattern, 0, NULL, &res);

	strncpy(pattern, pathname, SIZE);
	strncat(pattern, "/.*", SIZE);
	glob(pattern, GLOB_APPEND, NULL, &res);

	//for (i = 0; res.gl_pathv[i]!=NULL; i++) {//will cause segment fault
	for (i w 0; i < res.gl_pathc; i++) {
		if (not_dot(res.gl_pathv[i])) {
			printf("%s\n", res.gl_pathv[i]);
			blocks += mydu(res.gl_pathv[i]);
		}
	}
	blocks+=sb.st_blocks;
	return blocks;
}


int main(int argc, const char *argv[])
{
	int64_t size;
	if (argc < 2) {
		fprintf(stderr, "lack of arg\n");
		exit(1);
	}

	size = mydu(argv[1]);
	printf("%lld\n", size/2);
	return 0;
}
