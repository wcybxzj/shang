#include "event3/event-config.h"

#define _GNU_SOURCE
#include <sys/types.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _EVENT_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef _EVENT_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#ifdef _EVENT_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef _EVENT_HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif
#ifdef _EVENT_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef _EVENT_HAVE_GETTIMEOFDAY
#include <sys/timeb.h>
#include <time.h>
#endif
#include <sys/stat.h>

#include "event3/util.h"

int
evutil_snprintf(char *buf, size_t buflen, const char *format, ...)
{
    int r;
    va_list ap;
    va_start(ap, format);
    r = evutil_vsnprintf(buf, buflen, format, ap);
    va_end(ap);
    return r;
}

int
evutil_vsnprintf(char *buf, size_t buflen, const char *format, va_list ap)
{
    int r;
    if (!buflen)
        return 0;
    r = vsnprintf(buf, buflen, format, ap);
    buf[buflen-1] = '\0';
    return r;
}

