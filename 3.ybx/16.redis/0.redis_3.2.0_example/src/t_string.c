#include "server.h"
#include <math.h> /* isnan(), isinf() */


#define OBJ_SET_NO_FLAGS 0
#define OBJ_SET_NX (1<<0)     /* Set if key not exists. */          //在key不存在的情况下才会设置
#define OBJ_SET_XX (1<<1)     /* Set if key exists. */              //在key存在的情况下才会设置
#define OBJ_SET_EX (1<<2)     /* Set if time in seconds is given */ //以秒(s)为单位设置键的key过期时间
#define OBJ_SET_PX (1<<3)     /* Set if time in ms in given */      //以毫秒(ms)为单位设置键的key过期时间

//setGenericCommand()函数是以下命令: SET, SETEX, PSETEX, SETNX.的最底层实现
//flags 可以是NX或XX，由上面的宏提供
//expire 定义key的过期时间，格式由unit指定
//ok_reply和abort_reply保存着回复client的内容，NX和XX也会改变回复
//如果ok_reply为空，则使用 "+OK"
//如果abort_reply为空，则使用 "$-1"
void setGenericCommand(client *c, int flags, robj *key, robj *val, robj *expire, \
	int unit, robj *ok_reply, robj *abort_reply) {
    long long milliseconds = 0; /* initialized to avoid any harmness warning */ //初始化，避免错误

    ////如果定义了key的过期时间
    //if (expire) {
    //    //从expire对象中取出值，保存在milliseconds中，如果出错发送默认的信息给client
    //    if (getLongLongFromObjectOrReply(c, expire, &milliseconds, NULL) != C_OK)
    //        return;
    //    // 如果过期时间小于等于0，则发送错误信息给client
    //    if (milliseconds <= 0) {
    //        addReplyErrorFormat(c,"invalid expire time in %s",c->cmd->name);
    //        return;
    //    }
    //    //如果unit的单位是秒，则需要转换为毫秒保存
    //    if (unit == UNIT_SECONDS) milliseconds *= 1000;
    //}

    ////lookupKeyWrite函数是为执行写操作而取出key的值对象
    ////如果设置了NX(不存在)，并且在数据库中 找到 该key，或者
    ////设置了XX(存在)，并且在数据库中 没有找到 该key
    ////回复abort_reply给client
    //if ((flags & OBJ_SET_NX && lookupKeyWrite(c->db,key) != NULL) ||
    //    (flags & OBJ_SET_XX && lookupKeyWrite(c->db,key) == NULL))
    //{
    //    addReply(c, abort_reply ? abort_reply : shared.nullbulk);
    //    return;
    //}
    //在当前db设置键为key的值为val
    setKey(c->db,key,val);

    //设置数据库为脏(dirty)，服务器每次修改一个key后，都会对脏键(dirty)增1
    server.dirty++;

    //设置key的过期时间
    //mstime()返回毫秒为单位的格林威治时间
    //if (expire) setExpire(c->db,key,mstime()+milliseconds);

    ////发送"set"事件的通知，用于发布订阅模式，通知客户端接受发生的事件
    //notifyKeyspaceEvent(NOTIFY_STRING,"set",key,c->db->id);

    ////发送"expire"事件通知
    //if (expire) notifyKeyspaceEvent(NOTIFY_GENERIC,
    //    "exre",key,c->db->id);

    //设置成功，则向客户端发送ok_reply
	printf("%s\n",shared.ok->ptr);
    addReply(c, ok_reply ? ok_reply : shared.ok);
}



/* SET key value [NX] [XX] [EX <seconds>] [PX <milliseconds>] */
//SET命令
void setCommand(client *c) {
    int j;
    robj *expire = NULL;
    int unit = UNIT_SECONDS;        //单位为秒
    int flags = OBJ_SET_NO_FLAGS;   //初始化为0，表示默认为没有后面的[NX] [XX] [EX] [PX]参数

    //从第四个参数开始解析，
    for (j = 3; j < c->argc; j++) {
        char *a = c->argv[j]->ptr;  //保存第四个参数的首地址
        //EX和PX 参数后要 一个时间参数，next保存时间参数的地址
        robj *next = (j == c->argc-1) ? NULL : c->argv[j+1];

        //如果是 "nx" 或 "NX" 并且 flags没有设置 "XX" 的标志位
        if ((a[0] == 'n' || a[0] == 'N') &&
            (a[1] == 'x' || a[1] == 'X') && a[2] == '\0' &&
            !(flags & OBJ_SET_XX))
        {
            flags |= OBJ_SET_NX;    //设置 "NX" 标志位

        //如果是 "xx" 或 "XX" 并且 flags没有设置 "NX" 的标志位
        } else if ((a[0] == 'x' || a[0] == 'X') &&
                   (a[1] == 'x' || a[1] == 'X') && a[2] == '\0' &&
                   !(flags & OBJ_SET_NX))
        {
            flags |= OBJ_SET_XX;    //设置 "XX" 标志位

        //如果是 "ex" 或 "EX" 并且 flags没有设置 "PX" 的标志位，并且后面跟了时间
        } else if ((a[0] == 'e' || a[0] == 'E') &&
                   (a[1] == 'x' || a[1] == 'X') && a[2] == '\0' &&
                   !(flags & OBJ_SET_PX) && next)
        {
            flags |= OBJ_SET_EX;    //设置 "EX" 标志位
            unit = UNIT_SECONDS;    //EX 单位为秒
            expire = next;          //保存时间值
            j++;                    //跳过时间参数的下标

        //如果是 "px" 或 "PX" 并且 flags没有设置 "EX" 的标志位，并且后面跟了时间
        } else if ((a[0] == 'p' || a[0] == 'P') &&
                   (a[1] == 'x' || a[1] == 'X') && a[2] == '\0' &&
                   !(flags & OBJ_SET_EX) && next)
        {
            flags |= OBJ_SET_PX;    //设置 "PX" 标志位
            unit = UNIT_MILLISECONDS;//PX 单位为毫秒
            expire = next;           //保存时间值
            j++;                    //跳过时间参数的下标
        } else {
            //不是以上格式则回复client语法错误
            addReply(c,shared.syntaxerr);
            return;
        }
    }

    //对value进行最优的编码
    c->argv[2] = tryObjectEncoding(c->argv[2]);

    //调用底层的setGenericCommand函数实现SET命令
    setGenericCommand(c,flags,c->argv[1],c->argv[2],expire,unit,NULL,NULL);
}




void getCommand(client *c) {
	//TODO
}
