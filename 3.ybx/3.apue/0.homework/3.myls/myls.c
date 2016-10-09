#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define PATH_NUM 1
#define SIZE  1024

#define OPT_I (1<<0)
#define OPT_A (1<<1)
#define OPT_N (1<<2)
#define OPT_L (1<<3)

void myls(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	myls(argc, argv);
	return 0;
}

int is_dir(char *name)
{
	struct stat sb;
	if (stat(name, &sb) < 0) {
		perror("stat error!!!");
		exit(1);
	}

	if (S_ISDIR(sb.st_mode)) {
		return 1;
	}

	return 0;
}

void deal_path(char ***path, int *num,char *name)
{
	if (!(*num)) {
		(*path)[(*num)] = name;
	}else{
		*path = realloc(*path, sizeof(char **) * (*num));
		(*path)[*num] = name;
	}
	(*num)++;
}

void myls(int argc, char *argv[])
{
	char **path;
	int num = 0;
	glob_t res;
	char pattern[SIZE];
	int ch;
	int i;
	char opt = 0;

	struct stat sb;
	int inode = 0;
	char *mode;
	int hard_link = 0;
	int uid;
	int gid;
	char *username;
	char *groupname;
	struct tm *tm;
	char timestr[SIZE];

	struct passwd *pd;
	struct group *gp;

	path = malloc(sizeof(char **));
	if (NULL == path) {
		perror("malLoc():");
		exit(1);
	}
	path[0] = ".";

	while ((ch = getopt(argc, argv, "-ianl")) != -1) {
		switch (ch) {
			case 1:
				deal_path(&path, &num, argv[optind-1]);
				break;

			case 'i':
				opt = opt|OPT_I;
				break;

			case 'l':
				opt = opt|OPT_L;
				break;

			case 'a':
				opt = opt|OPT_A;
				break;

			case 'n':
				opt = opt|OPT_N;
				break;

			default: /* '?' */
				break;
				//fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
				//		argv[0]);
				//exit(EXIT_FAILURE);
		}
	}

	if (num==0) {
		path[0]=".";
		num = 1;
	}

	int dir_num = 0;
	for (i = 0; i < num; i++) {
		if (is_dir(path[i])) {
			if (dir_num == 0) {
				strncpy(pattern, path[i], SIZE);
				strncat(pattern, "/*", SIZE);
				glob(pattern, 0, NULL, &res);
			}else{
				strncpy(pattern, path[i], SIZE);
				strncat(pattern, "/*", SIZE);
				glob(pattern, GLOB_APPEND, NULL, &res);
			}
			if (opt&OPT_A) {
				strncpy(pattern, path[i], SIZE);
				strncat(pattern, "/.*", SIZE);
				glob(pattern, GLOB_APPEND, NULL, &res);
			}
			for (i = 0; i < res.gl_pathc; i++) {
				if (lstat(res.gl_pathv[i], &sb) < 0) {
					perror("stat()");
					exit(1);
				}
				if (opt&OPT_I) {
					printf("%ld ", (long)sb.st_ino);
				}

				if (opt&OPT_N || opt&OPT_L) {
					printf("%lo ", (unsigned long)sb.st_mode);
					printf("%ld ", (long)sb.st_nlink);
					if (opt&OPT_N) {
					   printf("%ld %ld ", (long) sb.st_uid, (long) sb.st_gid);
					}else if (opt&OPT_L) {
						pd = getpwuid(sb.st_uid);
						gp = getgrgid(sb.st_gid);
						printf("%s %s ", pd->pw_name, gp->gr_name);
					}
					tm = localtime(&sb.st_mtime);
					strftime(timestr, SIZE, "%m月 %d %H:%M", tm);
					printf("%lld %s %s\n", (long long)sb.st_size, timestr, res.gl_pathv[i]);
				}else{
					printf("%s \n", res.gl_pathv[i]);
				}
			}
			dir_num++;
		}else{
			if (lstat(path[i], &sb) < 0) {
				perror("stat()");
				exit(1);
			}
			if (opt&OPT_I) {
				printf("%ld ", (long)sb.st_ino);
			}

			if (opt&OPT_N || opt&OPT_L) {
				printf("%lo ", (unsigned long)sb.st_mode);
				printf("%ld ", (long)sb.st_nlink);
				if (opt&OPT_N) {
					printf("%ld %ld ", (long) sb.st_uid, (long) sb.st_gid);
				}else if (opt&OPT_L) {
					pd = getpwuid(sb.st_uid);
					gp = getgrgid(sb.st_gid);
					printf("%s %s ", pd->pw_name, gp->gr_name);
				}
				tm = localtime(&sb.st_mtime);
				strftime(timestr, SIZE, "%m月 %d %H:%M", tm);
				printf("%lld %s %s\n", (long long)sb.st_size, timestr,path[i]);
			}else{
				printf("%s\n", path[i]);
			}
		}
	}
	printf("\n");
}
