#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#define PAT "/etc"

//etc下有多少文件
int main(int argc, const char *argv[])
{
	DIR *dir;
	dir = opendir(PAT);
	if (NULL == dir) {
		perror("opendir():");
		exit(1);
	}

	struct dirent * mydirent;
	while (mydirent = readdir(dir)) {
		printf("%s\n", mydirent->d_name);
	}
	closedir(dir);
	return 0;
}
