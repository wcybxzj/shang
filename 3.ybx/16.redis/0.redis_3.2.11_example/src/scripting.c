#include "server.h"
#include "sha1.h"
#include "rand.h"
#include "cluster.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <ctype.h>
#include <math.h>

char *redisProtocolToLuaType_Int(lua_State *lua, char *reply);
char *redisProtocolToLuaType_Bulk(lua_State *lua, char *reply);
char *redisProtocolToLuaType_Status(lua_State *lua, char *reply);
char *redisProtocolToLuaType_Error(lua_State *lua, char *reply);
char *redisProtocolToLuaType_MultiBulk(lua_State *lua, char *reply);
int redis_math_random (lua_State *L);
int redis_math_randomseed (lua_State *L);
void ldbInit(void);
void ldbDisable(client *c);
void ldbEnable(client *c);
void evalGenericCommandWithDebugging(client *c, int evalsha);
void luaLdbLineHook(lua_State *lua, lua_Debug *ar);
void ldbLog(sds entry);
void ldbLogRedisReply(char *reply);
sds ldbCatStackValue(sds s, lua_State *lua, int idx);

/* Debugger shared state is stored inside this global structure. */
#define LDB_BREAKPOINTS_MAX 64  /* Max number of breakpoints. */
#define LDB_MAX_LEN_DEFAULT 256 /* Default len limit for replies / var dumps. */
struct ldbState {
    int fd;     /* Socket of the debugging client. */
    int active; /* Are we debugging EVAL right now? */
    int forked; /* Is this a fork()ed debugging session? */
    list *logs; /* List of messages to send to the client. */
    list *traces; /* Messages about Redis commands executed since last stop.*/
    list *children; /* All forked debugging sessions pids. */
    int bp[LDB_BREAKPOINTS_MAX]; /* An array of breakpoints line numbers. */
    int bpcount; /* Number of valid entries inside bp. */
    int step;   /* Stop at next line ragardless of breakpoints. */
    int luabp;  /* Stop at next line because redis.breakpoint() was called. */
    sds *src;   /* Lua script source code split by line. */
    int lines;  /* Number of lines in 'src'. */
    int currentline;    /* Current line number. */
    sds cbuf;   /* Debugger client command buffer. */
    size_t maxlen;  /* Max var dump / reply length. */
    int maxlen_hint_sent; /* Did we already hint about "set maxlen"? */
} ldb;


/* ---------------------------------------------------------------------------
 * Utility functions.
 * ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------
 * Redis reply to Lua type conversion functions.
 * ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------
 * Lua reply to Redis reply conversion functions.
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Lua redis.* functions implementations.
 * ------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------
 * Lua engine initialization and reset.
 * ------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------
 * Redis provided math.random
 * ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------
 * EVAL and SCRIPT commands implementation
 * ------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------
 * LDB: Redis Lua debugging facilities
 * ------------------------------------------------------------------------- */

/* Return the number of children we still did not received termination
 *  * acknowledge via wait() in the parent process. */
int ldbPendingChildren(void) {
	return 0;
    //return listLength(ldb.children);
}

