#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>

//#define PAT "/etc/a*.conf"
//#define PAT "/etc/.*" //查看隐藏文件
#define PAT "/etc/*"

static int errfunc_(const char *epath, int err){
	puts(epath);
	fprintf(stderr, "ERROR %s\n", strerror(err));
	return 0;
}

int main(int argc, const char *argv[])
{
	int i, err;
	glob_t res;
	err = glob(PAT, 0, NULL, &res);
	if (err) {
		printf("ERROR %d\n", err);
		exit(1);
	}

	//for (i = 0; res.gl_pathv[i]!=NULL ; i++) {
	for (i = 0; i <res.gl_pathc; i++) {
		printf("%s\n", res.gl_pathv[i]);
	}

	globfree(&res);

	return 0;
}
