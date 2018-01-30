#include "server.h"

/* ================================ MULTI/EXEC ============================== */

/* Client state initialization for MULTI/EXEC */
void initClientMultiState(client *c) {
	c->mstate.commands = NULL;
	c->mstate.count = 0;
}

/* Release all the resources associated with MULTI/EXEC state */
void freeClientMultiState(client *c) {
    int j;
    for (j = 0; j < c->mstate.count; j++) {
        int i;
        multiCmd *mc = c->mstate.commands+j;
        for (i = 0; i < mc->argc; i++)
            decrRefCount(mc->argv[i]);
        zfree(mc->argv);
    }
    zfree(c->mstate.commands);
}

/* Flag the transacation as DIRTY_EXEC so that EXEC will fail.
 * Should be called every time there is an error while queueing a command. */
// 设置事务的状态为DIRTY_EXEC，以便执行执行EXEC失败
// 每次在入队命令出错时调用
void flagTransaction(client *c) {
    if (c->flags & CLIENT_MULTI)
        // 如果客户端处于事务状态，设置DIRTY_EXEC标识
        c->flags |= CLIENT_DIRTY_EXEC;
}

