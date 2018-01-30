#include "server.h"

void notifyKeyspaceEvent(int type, char *event, robj *key, int dbid) {
	UNUSED(type);
	UNUSED(event);
	UNUSED(key);
	UNUSED(dbid);
}

