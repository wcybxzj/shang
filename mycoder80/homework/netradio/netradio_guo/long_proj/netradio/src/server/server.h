#ifndef SERVER_H__
#define SERVER_H__

#include <proto.h>

#define BUFSIZE		1024

extern int sd;
extern struct sockaddr_in raddr;
extern pthread_t tid[NR_CHN+1];

#endif
