#include "header.h"

int anetKeepAlive( int fd, int interval)
{
	int val = 1;

	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)
	{
		printf("setsockopt SO_KEEPALIVE: %s", strerror(errno));
		return ANET_ERR;
	}

#ifdef __linux__
	/* Default settings are more or less garbage, with the keepalive time
	 * set to 7200 by default on Linux. Modify settings to make the feature
	 * actually useful. */

	/* Send first probe after interval. */
	val = interval;
	if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) {
		printf("setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
		return ANET_ERR;
	}

	/* Send next probes after the specified interval. Note that we set the
	 * delay as interval / 3, as we send three probes before detecting
	 * an error (see the next setsockopt call). */
	val = interval/3;
	if (val == 0) val = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
		printf("setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));
		return ANET_ERR;
	}

	/* Consider the socket in error state after three we send three ACK
	 * probes without getting a reply. */
	val = 3;
	if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) {
		printf("setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
		return ANET_ERR;
	}
#else
	((void) interval); /* Avoid unused var warning for non Linux systems. */
#endif

	return ANET_OK;
}


int anetSetTcpNoDelay(int fd)
{
	int val=1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1)
	{
		printf("setsockopt TCP_NODELAY: %s\n", strerror(errno));
		return ANET_ERR;
	}
	return ANET_OK;
}
