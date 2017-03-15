#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid = "$OpenBSD: strlcpy.c,v 1.5 2001/05/13 15:40:16 deraadt Exp $";
#endif /* LIBC_SCCS and not lint */

#include <sys/types.h>

#include "event3/event-config.h"

#ifndef _EVENT_HAVE_STRLCPY
#include "strlcpy-internal.h"

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
_event_strlcpy(dst, src, siz)
	char *dst;
	const char *src;
	size_t siz;
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
		do {
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return (s - src - 1);	/* count does not include NUL */
}
#endif

