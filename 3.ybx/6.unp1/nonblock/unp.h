#ifndef __unp_h
#define __unp_h

#include "../config.h"

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
#include    <sys/stat.h>   
#include    <sys/uio.h>     /* for iovec{} and readv/writev */
#include    <unistd.h>
#include    <sys/wait.h>
#include    <sys/un.h>      /* for Unix domain sockets */

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

#define LISTENQ     1024    /* 2nd argument to listen() */
#define MAXLINE     4096    /* max text line length */
#define MAXSOCKADDR  128    /* max socket address structure size */
#define BUFFSIZE    8192    /* buffer size for reads and writes */

#define SERV_PORT        9877           /* TCP and UDP client-servers */
#define SERV_PORT_STR   "9877"          /* TCP and UDP client-servers */
#define UNIXSTR_PATH    "/tmp/unix.str" /* Unix domain stream cli-serv */
#define UNIXDG_PATH     "/tmp/unix.dg"  /* Unix domain datagram cli-serv */

#define SA  struct sockaddr

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE    (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)

typedef void    Sigfunc(int);   /* for signal handlers */

#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

int Tcp_connect(const char *host,\
		const char *port);

void     Close(int);


void     err_dump(const char *, ...);
void     err_msg(const char *, ...);
void     err_quit(const char *, ...);
void     err_ret(const char *, ...);
void     err_sys(const char *, ...);






# endif //end of __unp_h
