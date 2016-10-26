#include "unp.h"

#include <stdarg.h>
#include <syslog.h>

#define PRINT 1
#define NO_PRINT 0

int daemon_proc = 0;

static void err_doit(int print_error, int level, \
		const char *fmt, va_list ap){
	int errno_save, n;
	char buf[MAXLINE];
	
	errno_save = errno;

	vsnprintf(buf, sizeof(buf), fmt, ap);
	n = strlen(buf);
	if (print_error) {
		snprintf(buf+n, sizeof(buf)-n, \
				": %s", strerror(errno_save));
	}

	if (daemon_proc) {
		syslog(level, buf);
	}else{
		fprintf(stderr, "%s", buf);
	}
	return;
}

void err_ret(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(PRINT, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

//fatal error, exit
void err_sys(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(PRINT, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

//core dump
void err_dump(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(PRINT, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}

//not fatal error, no related to s system call
void err_msg(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(NO_PRINT, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

//fatal error, related to s system call
void err_quit(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	err_doit(NO_PRINT, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

//int main(int argc, const char *argv[])
//{
//	int fd = open("file_no_have.txt",O_RDONLY);
//	if (fd==-1) {
//		err_sys("nice123");
//	}
//	return 0;
//}
