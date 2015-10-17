#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <dirent.h>
#include <glob.h>

#define BUFSIZE	16
#define BUFMAX	1024
#define ENABL	(1 << 0)	
#define ENABI	(1 << 1)
#define ENABA	(1 << 2)
#define ENABN	(1 << 3)


int getmode(const mode_t mode, char **buf)
{	
	char *str = NULL;

	str = malloc(BUFSIZE);
	if(NULL == str)
		return -1;
	memset(str, 0x00, BUFSIZE);
	if(S_ISDIR(mode))
		strncpy(str, "d", BUFSIZE);
	else if(S_ISREG(mode))
		strncpy(str, "-", BUFSIZE);
	else if(S_ISCHR(mode))
		strncpy(str, "c", BUFSIZE);
	else if(S_ISBLK(mode))
		strncpy(str, "b", BUFSIZE);
	else if(S_ISFIFO(mode))
		strncpy(str, "p", BUFSIZE);
	else if(S_ISLNK(mode))
		strncpy(str, "l", BUFSIZE);
	else if(S_ISSOCK(mode))
		strncpy(str, "s", BUFSIZE);
	else 
		return -1;

	if(mode & S_IRUSR)
		strncat(str, "r", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IWUSR)
		strncat(str, "w", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IXUSR)
		strncat(str, "x", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IRGRP)
		strncat(str, "r", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IWGRP)
		strncat(str, "w", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IXGRP)
		strncat(str, "x", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IROTH)
		strncat(str, "r", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IWOTH)
		strncat(str, "w", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);
	if(mode & S_IXOTH)
		strncat(str, "x", BUFSIZE);
	else
		strncat(str, "-", BUFSIZE);

	free(*buf);
	*buf = str;

	return 0;
}

int gettime(const time_t *stamp, char **buf)
{
	struct tm *tm;
	char *str = NULL;

	str = malloc(BUFSIZE);
	if(NULL == str)
		return -1;
	tm = localtime(stamp);
	if(NULL == tm)
		return -1;
	memset(str, 0x00, BUFSIZE);
	strftime(str, BUFSIZE, "%m月 %d %H:%M", tm);
	free(*buf);
	*buf = str;

	return 0;
}

int isdir(const char *path)
{
	struct stat statbuf;

	if(lstat(path, &statbuf) < 0)
		return -1;
	if(S_ISDIR(statbuf.st_mode))
		return 1;
	else 
		return 0;
}

int main(int argc, char **argv)
{
	int ch, i;
	char *path;
	int dirflag;
	char optflag = 0;
	char *mode = NULL;
	char *time = NULL;
	struct stat statbuf;
	struct passwd *pw;
	struct group *gr;
	glob_t globbuf;
	char nextpath[BUFMAX] = {};
	

	while(1) {
		ch = getopt(argc, argv, "-lain");
		if(ch < 0)
			break;
		switch(ch) {
			case 1:
				dirflag = isdir(argv[optind-1]);
				if(dirflag < 0) {
					fprintf(stderr, "Invalid argument\n");
					exit(1);
				}
				path = argv[optind-1];
				break;
			case 'l':
				optflag |= ENABL;
				break;
			case 'a':
				optflag |= ENABA;
				break;
			case 'i':
				optflag |= ENABI;
				break;
			case 'n':
				optflag |= ENABN;
				break;
			default:
				break;
		}
	}

	if(dirflag) {
		if(optflag & ENABA) {
			strncpy(nextpath, path, BUFMAX);
			strncat(nextpath, "/*", BUFMAX);
			if(glob(nextpath, 0, NULL, &globbuf)) {
				fprintf(stderr, "glob() is error\n");
				exit(1);
			}
			strncpy(nextpath, path, BUFMAX);
			strncat(nextpath, "/.*", BUFMAX);
			if(glob(nextpath, GLOB_APPEND, NULL, &globbuf)) {
				fprintf(stderr, "glob() is error\n");
				exit(1);
			}
		}
		else {
			strncpy(nextpath, path, BUFMAX);
			strncat(nextpath, "/*", BUFMAX);
			if(glob(nextpath, 0, NULL, &globbuf)) {
				fprintf(stderr, "glob() is error\n");
				exit(1);
			}
		}
		for(i = 0; i < globbuf.gl_pathc; i++) {
			if(lstat(globbuf.gl_pathv[i], &statbuf) < 0) {
				perror("lstat()");
				exit(1);
			}
			pw = getpwuid(statbuf.st_uid);
			if(NULL == pw) {
				perror("getpwuid()");
				exit(1);
			}
			gr = getgrgid(statbuf.st_gid);
			if(NULL == gr) {
				perror("getgrgid()");
				exit(1);
			}
			if(getmode(statbuf.st_mode, &mode) < 0) {
				fprintf(stderr, "getmode() is error\n");
				exit(1);
			}
			if(gettime(&statbuf.st_mtime, &time) < 0) {
				fprintf(stderr, "gettime() is error\n");
				exit(1);
			}
			if(optflag & ENABI) {
				printf("%lld ", (long long)statbuf.st_ino);
			}
			if(optflag & ENABL) {
				if(optflag & ENABN) {	
					printf("%s %lld %lld %lld %lld %s ", mode,\
							(long long)statbuf.st_nlink,\
							(long long)statbuf.st_uid, \
							(long long)statbuf.st_gid, \
							(long long)statbuf.st_size, time);
				}
				else {
					printf("%s %lld %s %s %lld %s ", mode,\
							(long long)statbuf.st_nlink,\
							pw->pw_name, gr->gr_name, \
							(long long)statbuf.st_size, time);
				}
			}
			printf("%s\n", globbuf.gl_pathv[i]);
			free(mode);	mode = NULL;
			free(time);	time = NULL;
		}
	}else {
		if(lstat(path, &statbuf) < 0) {
			perror("lstat()");
			exit(1);
		}
		pw = getpwuid(statbuf.st_uid);
		if(NULL == pw) {
			perror("getpwuid()");
			exit(1);
		}
		gr = getgrgid(statbuf.st_gid);
		if(NULL == gr) {
			perror("getgrgid()");
			exit(1);
		}
		if(getmode(statbuf.st_mode, &mode) < 0) {
			fprintf(stderr, "getmode() is error\n");
			exit(1);
		}
		if(gettime(&statbuf.st_mtime, &time) < 0) {
			fprintf(stderr, "gettime() is error\n");
			exit(1);
		}
		if(optflag & ENABI) {
			printf("%lld ", (long long)statbuf.st_ino);
		}
		if(optflag & ENABL) {
			if(optflag & ENABN) {	
				printf("%s %lld %lld %lld %lld %s ", mode,\
					   (long long)statbuf.st_nlink,\
					   (long long)statbuf.st_uid, \
					   (long long)statbuf.st_gid, \
					   (long long)statbuf.st_size, time);
			}
			else {
				printf("%s %lld %s %s %lld %s ", mode,\
					   (long long)statbuf.st_nlink,\
					   pw->pw_name, gr->gr_name, \
					   (long long)statbuf.st_size, time);
			}
		}
		printf("%s\n", path);
		free(mode); mode = NULL;
		free(time); time = NULL;
	}

	exit(0);
}
