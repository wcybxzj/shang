#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define SIZE 1024

static int64_t mydu(const char *path)
{
	int64_t blocks = 0;
	struct stat sb;
	char tmp_path[SIZE];
	struct dirent *mydirent;
	DIR *dir;
	if(lstat(path, &sb)<0){
		perror("lstat():");
		exit(1);
	
	}

	if (!S_ISDIR(sb.st_mode)) {
		return sb.st_blocks;
	}

	blocks += sb.st_blocks;
	dir = opendir(path);
	if (NULL == dir) {
		perror("opendir()");
		exit(1);
	}
	
	while (mydirent = readdir(dir)) {
		if (strcmp(mydirent->d_name, ".")==0 || strcmp(mydirent->d_name, "..")==0) {
			continue;
		}
		strcpy(tmp_path, path);
		strcat(tmp_path, "/");
		strcat(tmp_path, mydirent->d_name);
		blocks += mydu(tmp_path);
	}
	return blocks;

}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}
	int64_t size;
	size = mydu(argv[1]);
	printf("%lld\n", size/2);
	return 0;
}
