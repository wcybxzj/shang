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

#include <event3/util.h>

char EVUTIL_TOUPPER(char c);

int
evutil_socketpair(int family, int type, int protocol, evutil_socket_t fd[2])
{
#ifndef WIN32
	return socketpair(family, type, protocol, fd);
#else
	return evutil_ersatz_socketpair(family, type, protocol, fd);
#endif
}

int
evutil_make_socket_nonblocking(evutil_socket_t fd)
{
#ifdef WIN32
	{
		u_long nonblocking = 1;
		if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
			event_sock_warn(fd, "fcntl(%d, F_GETFL)", (int)fd);
			return -1;
		}
	}
#else
	{
		int flags;
		if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
			event_warn("fcntl(%d, F_GETFL)", fd);
			return -1;
		}
		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			event_warn("fcntl(%d, F_SETFL)", fd);
			return -1;
		}
	}
#endif
	return 0;
}

int
evutil_make_socket_closeonexec(evutil_socket_t fd)
{
#if !defined(WIN32) && defined(_EVENT_HAVE_SETFD)
	int flags;
	if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
		event_warn("fcntl(%d, F_GETFD)", fd);
		return -1;
	}
	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
		event_warn("fcntl(%d, F_SETFD)", fd);
		return -1;
	}
#endif
	return 0;
}

int
evutil_closesocket(evutil_socket_t sock)
{
#ifndef WIN32
	return close(sock);
#else
	return closesocket(sock);
#endif
}

#ifndef _EVENT_HAVE_GETTIMEOFDAY
/* No gettimeofday; this muse be windows. */
int
evutil_gettimeofday(struct timeval *tv, struct timezone *tz) 
{
    struct _timeb tb;

    if (tv == NULL)
        return -1;

    /* XXXX
     * _ftime is not the greatest interface here; GetSystemTimeAsFileTime
     * would give us better resolution, whereas something cobbled together
     * with GetTickCount could maybe give us monotonic behavior.
     *
     * Either way, I think this value might be skewed to ignore the
     * timezone, and just return local time.  That's not so good.
     */
    _ftime(&tb);
    tv->tv_sec = (long) tb.time;
    tv->tv_usec = ((int) tb.millitm) * 1000;
    return 0;
}
#endif

#ifdef WIN32
int
evutil_socket_geterror(evutil_socket_t sock)
{
	int optval, optvallen=sizeof(optval);
	int err = WSAGetLastError();
	if (err == WSAEWOULDBLOCK && sock >= 0) {
		if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)&optval,
					   &optvallen))
			return err;
		if (optval)
			return optval;
	}
	return err;
}
#endif

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

static const unsigned char EVUTIL_TOUPPER_TABLE[256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

static const unsigned char EVUTIL_TOLOWER_TABLE[256] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};

char EVUTIL_TOUPPER(char c)
{
	return ((char)EVUTIL_TOUPPER_TABLE[(ev_uint8_t)c]);
}

static int
evutil_issetugid(void)
{
#ifdef _EVENT_HAVE_ISSETUGID
	return issetugid();
#else

#ifdef _EVENT_HAVE_GETEUID
	if (getuid() != geteuid())
		return 1;
#endif
#ifdef _EVENT_HAVE_GETEGID
	if (getgid() != getegid())
		return 1;
#endif
	return 0;
#endif
}

const char *
evutil_getenv(const char *varname)
{
	if (evutil_issetugid())
		return NULL;

	return getenv(varname);
}
