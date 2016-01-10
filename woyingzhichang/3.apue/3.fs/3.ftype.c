#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

//S_ISREG(m)  is it a regular file?
//S_ISDIR(m)  directory?
//S_ISCHR(m)  character device?
//S_ISBLK(m)  block device?
//S_ISFIFO(m) FIFO (named pipe)?
//S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
//S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
static int ftype(const char *fname)
{
	struct stat buf; 
	if (stat(fname, &buf) <0 ) {
		perror("stat()");
		exit(1);
	}
	if (S_ISREG(buf.st_mode)) {
		return '-';
	}else if(S_ISDIR(buf.st_mode)){
		return 'd';
	}else if(S_ISCHR(buf.st_mode)){
		return 'c';
	}else if(S_ISBLK(buf.st_mode)){
		return 'b';
	}else if(S_ISFIFO(buf.st_mode)){
		return 'p';
	}else if(S_ISLNK(buf.st_mode)){
		return 'l';
	}else if(S_ISSOCK(buf.st_mode)){
		return 's';
	}else{
		return '?';
	}
}

/*
S_IFMT     0170000   bit mask for the file type bit fields

S_IFSOCK   0140000   socket
S_IFLNK    0120000   symbolic link
S_IFREG    0100000   regular file
S_IFBLK    0060000   block device
S_IFDIR    0040000   directory
S_IFCHR    0020000   character device
S_IFIFO    0010000   FIFO

if ((sb.st_mode & S_IFMT) == S_IFREG) {
	 Handle regular file
}
*/
static int another_ftype(const char *fname)
{
	struct stat sb;

	if (stat(fname, &sb) < 0) {
		perror("stat():");
		exit(1);
	}

	if ((sb.st_mode & S_IFMT) == S_IFREG) {
		return '-';
	}else if ((sb.st_mode & S_IFMT) == S_IFSOCK) {
		return 's';
	}else if ((sb.st_mode & S_IFMT) == S_IFLNK ) {
		return 'l';
	}else if ((sb.st_mode & S_IFMT) == S_IFBLK ) {
		return 'b';
	}else if ((sb.st_mode & S_IFMT) == S_IFDIR ) {
		return 'd';
	}else if ((sb.st_mode & S_IFMT) == S_IFCHR ) {
		return 'c';
	}else if ((sb.st_mode & S_IFMT) == S_IFIFO ) {
		return 'p';
	}else{
		return '?';
	}
}


int main(int argc, const char *argv[])
{
	char ret;
	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	ret = ftype(argv[1]);
	printf("%s is %c\n", argv[0], ret);

	ret = another_ftype(argv[1]);
	printf("%s is %c\n", argv[0], ret);


	return 0;
}
