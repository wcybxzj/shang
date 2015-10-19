#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <glob.h>

#define PATHSIZE 1024

int path_noloop(const char *path)
{
	//  /a/b/c/d/e/...a...
	char *pos;

	pos = strrchr(path,'/');
	if(strcmp(pos+1,".") == 0 || strcmp(pos+1,"..") == 0)
		return 0;
	return 1;
}

int64_t mydu(const char *path)
{// ../a/b/c/d/e
	struct stat statres;
	char nextpath[PATHSIZE];
	glob_t globres;
	int i;
	int64_t sum = 0;
	DIR  *dp;
	struct dirent *cur = NULL;

	if(lstat(path,&statres) < 0)
	{
		printf("path %s\n", path);
		perror("lstat()");
		exit(1);
	}

	if(!S_ISDIR(statres.st_mode))
		return statres.st_blocks;

	// is a dir
	// "../a/b/c/d/e/*"  "../a/b/c/d/e/.*"
	dp = opendir(path);
	if (dp == NULL) {
		perror("opendit()");
		exit(1);
	}

	strncpy(nextpath,path,PATHSIZE);
	while ( (cur = readdir(dp)) != NULL ) {
		strncat(nextpath, "/", PATHSIZE);
		strncat(nextpath, cur->d_name, PATHSIZE);
		printf("============\n");
		printf("%s\n", cur->d_name);
		if(path_noloop(nextpath))
        	sum += mydu(nextpath);
	}

	close(dp);

	sum += statres.st_blocks;

	return sum;
}

int main(int argc,char **argv)
{

	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
	
	printf("%lld\n",mydu(argv[1])/2);

	exit(0);
}

