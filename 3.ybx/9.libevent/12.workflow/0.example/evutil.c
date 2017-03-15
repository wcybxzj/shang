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
#include "util-internal.h"
#include "log-internal.h"
#include "mm-internal.h"

#include "strlcpy-internal.h"
#include "ipv6-internal.h"

int EVUTIL_ISALPHA(char c);
int EVUTIL_ISALNUM(char c);
int EVUTIL_ISSPACE(char c);
int EVUTIL_ISDIGIT(char c);
int EVUTIL_ISXDIGIT(char c);
int EVUTIL_ISPRINT(char c);
int EVUTIL_ISLOWER(char c);
int EVUTIL_ISUPPER(char c);
char EVUTIL_TOUPPER(char c);
char EVUTIL_TOLOWER(char c);

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
evutil_make_listen_socket_reuseable(evutil_socket_t sock)
{
#ifndef WIN32
	int one = 1;
	/* REUSEADDR on Unix means, "don't hang on to this address after the
	 * listener is closed."  On Windows, though, it means "don't keep other
	 * processes from binding to this address while we're using it. */
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*) &one,
	    (ev_socklen_t)sizeof(one));
#else
	return 0;
#endif
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
evutil_socket_finished_connecting(evutil_socket_t fd)
{
	int e;
	ev_socklen_t elen = sizeof(e);

	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (void*)&e, &elen) < 0)
		return -1;

	if (e) {
		if (EVUTIL_ERR_CONNECT_RETRIABLE(e))
			return 0;
		EVUTIL_SET_SOCKET_ERROR(e);
		return -1;
	}

	return 1;
}

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

#define USE_INTERNAL_NTOP
#define USE_INTERNAL_PTON

const char *
evutil_inet_ntop(int af, const void *src, char *dst, size_t len)
{
#if defined(_EVENT_HAVE_INET_NTOP) && !defined(USE_INTERNAL_NTOP)
	return inet_ntop(af, src, dst, len);
#else
	if (af == AF_INET) {
		const struct in_addr *in = src;
		const ev_uint32_t a = ntohl(in->s_addr);
		int r;
		r = evutil_snprintf(dst, len, "%d.%d.%d.%d",
		    (int)(ev_uint8_t)((a>>24)&0xff),
		    (int)(ev_uint8_t)((a>>16)&0xff),
		    (int)(ev_uint8_t)((a>>8 )&0xff),
		    (int)(ev_uint8_t)((a    )&0xff));
		if (r<0||(size_t)r>=len)
			return NULL;
		else
			return dst;
#ifdef AF_INET6
	} else if (af == AF_INET6) {
		const struct in6_addr *addr = src;
		char buf[64], *cp;
		int longestGapLen = 0, longestGapPos = -1, i,
			curGapPos = -1, curGapLen = 0;
		ev_uint16_t words[8];
		for (i = 0; i < 8; ++i) {
			words[i] =
			    (((ev_uint16_t)addr->s6_addr[2*i])<<8) + addr->s6_addr[2*i+1];
		}
		if (words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
		    words[4] == 0 && ((words[5] == 0 && words[6] && words[7]) ||
			(words[5] == 0xffff))) {
			/* This is an IPv4 address. */
			if (words[5] == 0) {
				evutil_snprintf(buf, sizeof(buf), "::%d.%d.%d.%d",
				    addr->s6_addr[12], addr->s6_addr[13],
				    addr->s6_addr[14], addr->s6_addr[15]);
			} else {
				evutil_snprintf(buf, sizeof(buf), "::%x:%d.%d.%d.%d", words[5],
				    addr->s6_addr[12], addr->s6_addr[13],
				    addr->s6_addr[14], addr->s6_addr[15]);
			}
			if (strlen(buf) > len)
				return NULL;
			strlcpy(dst, buf, len);
			return dst;
		}
		i = 0;
		while (i < 8) {
			if (words[i] == 0) {
				curGapPos = i++;
				curGapLen = 1;
				while (i<8 && words[i] == 0) {
					++i; ++curGapLen;
				}
				if (curGapLen > longestGapLen) {
					longestGapPos = curGapPos;
					longestGapLen = curGapLen;
				}
			} else {
				++i;
			}
		}
		if (longestGapLen<=1)
			longestGapPos = -1;

		cp = buf;
		for (i = 0; i < 8; ++i) {
			if (words[i] == 0 && longestGapPos == i) {
				if (i == 0)
					*cp++ = ':';
				*cp++ = ':';
				while (i < 8 && words[i] == 0)
					++i;
				--i; /* to compensate for loop increment. */
			} else {
				evutil_snprintf(cp,
								sizeof(buf)-(cp-buf), "%x", (unsigned)words[i]);
				cp += strlen(cp);
				if (i != 7)
					*cp++ = ':';
			}
		}
		*cp = '\0';
		if (strlen(buf) > len)
			return NULL;
		strlcpy(dst, buf, len);
		return dst;
#endif
	} else {
		return NULL;
	}
#endif
}//end of evutil_inet_ntop


int
evutil_inet_pton(int af, const char *src, void *dst)
{
#if defined(_EVENT_HAVE_INET_PTON) && !defined(USE_INTERNAL_PTON)
	return inet_pton(af, src, dst);
#else
	if (af == AF_INET) {
		int a,b,c,d;
		char more;
		struct in_addr *addr = dst;
		if (sscanf(src, "%d.%d.%d.%d%c", &a,&b,&c,&d,&more) != 4)
			return 0;
		if (a < 0 || a > 255) return 0;
		if (b < 0 || b > 255) return 0;
		if (c < 0 || c > 255) return 0;
		if (d < 0 || d > 255) return 0;
		addr->s_addr = htonl((a<<24) | (b<<16) | (c<<8) | d);
		return 1;
#ifdef AF_INET6
	} else if (af == AF_INET6) {
		struct in6_addr *out = dst;
		ev_uint16_t words[8];
		int gapPos = -1, i, setWords=0;
		const char *dot = strchr(src, '.');
		const char *eow; /* end of words. */
		if (dot == src)
			return 0;
		else if (!dot)
			eow = src+strlen(src);
		else {
			int byte1,byte2,byte3,byte4;
			char more;
			for (eow = dot-1; eow >= src && EVUTIL_ISDIGIT(*eow); --eow)
				;
			++eow;

			/* We use "scanf" because some platform inet_aton()s are too lax
			 * about IPv4 addresses of the form "1.2.3" */
			if (sscanf(eow, "%d.%d.%d.%d%c",
					   &byte1,&byte2,&byte3,&byte4,&more) != 4)
				return 0;

			if (byte1 > 255 || byte1 < 0 ||
				byte2 > 255 || byte2 < 0 ||
				byte3 > 255 || byte3 < 0 ||
				byte4 > 255 || byte4 < 0)
				return 0;

			words[6] = (byte1<<8) | byte2;
			words[7] = (byte3<<8) | byte4;
			setWords += 2;
		}

		i = 0;
		while (src < eow) {
			if (i > 7)
				return 0;
			if (EVUTIL_ISXDIGIT(*src)) {
				char *next;
				long r = strtol(src, &next, 16);
				if (next > 4+src)
					return 0;
				if (next == src)
					return 0;
				if (r<0 || r>65536)
					return 0;

				words[i++] = (ev_uint16_t)r;
				setWords++;
				src = next;
				if (*src != ':' && src != eow)
					return 0;
				++src;
			} else if (*src == ':' && i > 0 && gapPos==-1) {
				gapPos = i;
				++src;
			} else if (*src == ':' && i == 0 && src[1] == ':' && gapPos==-1) {
				gapPos = i;
				src += 2;
			} else {
				return 0;
			}
		}

		if (setWords > 8 ||
			(setWords == 8 && gapPos != -1) ||
			(setWords < 8 && gapPos == -1))
			return 0;

		if (gapPos >= 0) {
			int nToMove = setWords - (dot ? 2 : 0) - gapPos;
			int gapLen = 8 - setWords;
			/* assert(nToMove >= 0); */
			if (nToMove < 0)
				return -1; /* should be impossible */
			memmove(&words[gapPos+gapLen], &words[gapPos],
					sizeof(ev_uint16_t)*nToMove);
			memset(&words[gapPos], 0, sizeof(ev_uint16_t)*gapLen);
		}
		for (i = 0; i < 8; ++i) {
			out->s6_addr[2*i  ] = words[i] >> 8;
			out->s6_addr[2*i+1] = words[i] & 0xff;
		}

		return 1;
#endif
	} else {
		return -1;
	}
#endif
}// end  evutil_inet_pton


int
evutil_parse_sockaddr_port(const char *ip_as_string, struct sockaddr *out, int *outlen)
{
	int port;
	char buf[128];
	const char *cp, *addr_part, *port_part;
	int is_ipv6;
	/* recognized formats are:
	 * [ipv6]:port
	 * ipv6
	 * [ipv6]
	 * ipv4:port
	 * ipv4
	 */

	cp = strchr(ip_as_string, ':');
	if (*ip_as_string == '[') {
		int len;
		if (!(cp = strchr(ip_as_string, ']'))) {
			return -1;
		}
		len = (int) ( cp-(ip_as_string + 1) );
		if (len > (int)sizeof(buf)-1) {
			return -1;
		}
		memcpy(buf, ip_as_string+1, len);
		buf[len] = '\0';
		addr_part = buf;
		if (cp[1] == ':')
			port_part = cp+2;
		else
			port_part = NULL;
		is_ipv6 = 1;
	} else if (cp && strchr(cp+1, ':')) {
		is_ipv6 = 1;
		addr_part = ip_as_string;
		port_part = NULL;
	} else if (cp) {
		is_ipv6 = 0;
		if (cp - ip_as_string > (int)sizeof(buf)-1) {
			return -1;
		}
		memcpy(buf, ip_as_string, cp-ip_as_string);
		buf[cp-ip_as_string] = '\0';
		addr_part = buf;
		port_part = cp+1;
	} else {
		addr_part = ip_as_string;
		port_part = NULL;
		is_ipv6 = 0;
	}

	if (port_part == NULL) {
		port = 0;
	} else {
		port = atoi(port_part);
		if (port <= 0 || port > 65535) {
			return -1;
		}
	}

	if (!addr_part)
		return -1; /* Should be impossible. */
#ifdef AF_INET6
	if (is_ipv6)
	{
		struct sockaddr_in6 sin6;
		memset(&sin6, 0, sizeof(sin6));
#ifdef _EVENT_HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN
		sin6.sin6_len = sizeof(sin6);
#endif
		sin6.sin6_family = AF_INET6;
		sin6.sin6_port = htons(port);
		if (1 != evutil_inet_pton(AF_INET6, addr_part, &sin6.sin6_addr))
			return -1;
		if ((int)sizeof(sin6) > *outlen)
			return -1;
		memset(out, 0, *outlen);
		memcpy(out, &sin6, sizeof(sin6));
		*outlen = sizeof(sin6);
		return 0;
	}
	else
#endif
	{
		struct sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
#ifdef _EVENT_HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
		sin.sin_len = sizeof(sin);
#endif
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
		if (1 != evutil_inet_pton(AF_INET, addr_part, &sin.sin_addr))
			return -1;
		if ((int)sizeof(sin) > *outlen)
			return -1;
		memset(out, 0, *outlen);
		memcpy(out, &sin, sizeof(sin));
		*outlen = sizeof(sin);
		return 0;
	}
}// end  evutil_parse_sockaddr_port

static const ev_uint32_t EVUTIL_ISALPHA_TABLE[8] =
  { 0, 0, 0x7fffffe, 0x7fffffe, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISALNUM_TABLE[8] =
  { 0, 0x3ff0000, 0x7fffffe, 0x7fffffe, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISSPACE_TABLE[8] = { 0x3e00, 0x1, 0, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISXDIGIT_TABLE[8] =
  { 0, 0x3ff0000, 0x7e, 0x7e, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISDIGIT_TABLE[8] = { 0, 0x3ff0000, 0, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISPRINT_TABLE[8] =
  { 0, 0xffffffff, 0xffffffff, 0x7fffffff, 0, 0, 0, 0x0 };
static const ev_uint32_t EVUTIL_ISUPPER_TABLE[8] = { 0, 0, 0x7fffffe, 0, 0, 0, 0, 0 };
static const ev_uint32_t EVUTIL_ISLOWER_TABLE[8] = { 0, 0, 0, 0x7fffffe, 0, 0, 0, 0 };

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
#define IMPL_CTYPE_FN(name)						\
	int EVUTIL_##name(char c) {					\
		ev_uint8_t u = c;					\
		return !!(EVUTIL_##name##_TABLE[(u >> 5) & 7] & (1 << (u & 31))); \
	}
IMPL_CTYPE_FN(ISALPHA)
IMPL_CTYPE_FN(ISALNUM)
IMPL_CTYPE_FN(ISSPACE)
IMPL_CTYPE_FN(ISDIGIT)
IMPL_CTYPE_FN(ISXDIGIT)
IMPL_CTYPE_FN(ISPRINT)
IMPL_CTYPE_FN(ISLOWER)
IMPL_CTYPE_FN(ISUPPER)

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

long
_evutil_weakrand(void)
{
#ifdef WIN32
	return rand();
#else
	return random();
#endif
}

#define MAX_SECONDS_IN_MSEC_LONG \
	(((LONG_MAX) - 999) / 1000)


long
evutil_tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
		return -1;

	return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
}

