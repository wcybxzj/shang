#include <event3/event-config.h>

#include <sys/types.h>
#if !defined(WIN32) && defined(_EVENT_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif
#include <sys/queue.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _EVENT_HAVE_SYS_EVENTFD_H
#include <sys/eventfd.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#ifndef _EVENT_DISABLE_MM_REPLACEMENT
static void *(*_mm_malloc_fn)(size_t sz) = NULL;
static void *(*_mm_realloc_fn)(void *p, size_t sz) = NULL;
static void (*_mm_free_fn)(void *p) = NULL;

void *
event_mm_malloc_(size_t sz)
{
    if (_mm_malloc_fn)
        return _mm_malloc_fn(sz);
    else
        return malloc(sz);
}

void *
event_mm_calloc_(size_t count, size_t size)
{
    if (_mm_malloc_fn) {
        size_t sz = count * size;
        void *p = _mm_malloc_fn(sz);
        if (p)
            memset(p, 0, sz);
        return p;
    } else
        return calloc(count, size);
}

char *                                                                                                                                                                              
event_mm_strdup_(const char *str)
{
    if (_mm_malloc_fn) {
        size_t ln = strlen(str);
        void *p = _mm_malloc_fn(ln+1);
        if (p)
            memcpy(p, str, ln+1);
        return p;
	} else{
		return strdup(str);
	}
}

void *
event_mm_realloc_(void *ptr, size_t sz)
{
	if (_mm_realloc_fn)
		return _mm_realloc_fn(ptr, sz);
	else
		return realloc(ptr, sz);
}

void
event_mm_free_(void *ptr)
{
	if (_mm_free_fn)
		_mm_free_fn(ptr);
	else
		free(ptr);
}

void                                                                                                                                                                                
event_set_mem_functions(void *(*malloc_fn)(size_t sz),
            void *(*realloc_fn)(void *ptr, size_t sz),
            void (*free_fn)(void *ptr))
{
    _mm_malloc_fn = malloc_fn;
    _mm_realloc_fn = realloc_fn;
    _mm_free_fn = free_fn;
}
#endif

