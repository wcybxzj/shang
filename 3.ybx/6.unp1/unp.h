#ifndef __unp_h
#include "config.h"

#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#include    <sys/time.h>    /* timeval{} for select() */
#include    <time.h>        /* timespec{} for pselect() */
#include    <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include    <arpa/inet.h>   /* inet(3) functions */
#include    <errno.h>
#include    <fcntl.h>       /* for nonblocking */
#include    <netdb.h>
#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/stat.h>    /* for S_xxx file mode constants */
#include    <sys/uio.h>     /* for iovec{} and readv/writev */
#include    <unistd.h>
#include    <sys/wait.h>
#include    <sys/un.h>      /* for Unix domain sockets */
#include <libgen.h>

#ifdef  HAVE_SYS_SELECT_H
# include   <sys/select.h>  /* for convenience */
#endif

#ifdef  HAVE_POLL_H
# include   <poll.h>        /* for convenience */
#endif

#ifdef  HAVE_STRINGS_H
# include   <strings.h>     /* for convenience */
#endif

#ifdef  HAVE_SYS_IOCTL_H
# include   <sys/ioctl.h>
#endif                                                                                   
#ifdef  HAVE_SYS_FILIO_H
# include   <sys/filio.h>
#endif
#ifdef  HAVE_SYS_SOCKIO_H
# include   <sys/sockio.h>
#endif

#ifdef  HAVE_PTHREAD_H
# include   <pthread.h>
#endif

/* OSF/1 actually disables recv() and send() in <sys/socket.h> */
#ifdef  __osf__
#undef  recv
#undef  send
#define recv(a,b,c,d)   recvfrom(a,b,c,d,0,0)
#define send(a,b,c,d)   sendto(a,b,c,d,0,0)
#endif

#define LISTENQ     1024    /* 2nd argument to listen() */

/* Miscellaneous constants */
#define MAXLINE     4096    /* max text line length */
#define MAXSOCKADDR  128    /* max socket address structure size */
#define BUFFSIZE    8192    /* buffer size for reads and writes */

/* Define some port number that can be used for client-servers */
#define SERV_PORT        9877           /* TCP and UDP client-servers */
#define SERV_PORT_STR   "9877"          /* TCP and UDP client-servers */
#define UNIXSTR_PATH    "/tmp/unix.str" /* Unix domain stream cli-serv */
#define UNIXDG_PATH     "/tmp/unix.dg"  /* Unix domain datagram cli-serv */

/* Following shortens all the type casts of pointer arguments */
#define SA  struct sockaddr

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
                    /* default file access permissions for new files */
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
                    /* default permissions for new directories */

typedef void    Sigfunc(int);   /* for signal handlers */

#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

int     tcp_connect(const char *, const char *);
ssize_t writen(int fd, const void *ptr, size_t n);
ssize_t readn(int fd, void *ptr, size_t n);


struct	addrinfo * host_serv(const char *host, const char *serv, \
		int family, int socktype);
struct	addrinfo * Host_serv(const char *host, const char *serv, \
		int family, int socktype);
void	Writen(int fd, const void *ptr, size_t n);
ssize_t Readn(int fd, void *ptr, size_t n);
void	Close(int fd);
int     Tcp_connect(const char *, const char *);

void     err_dump(const char *, ...);
void     err_msg(const char *, ...);
void     err_quit(const char *, ...);
void     err_ret(const char *, ...);
void     err_sys(const char *, ...);


#endif //end __unp_h
