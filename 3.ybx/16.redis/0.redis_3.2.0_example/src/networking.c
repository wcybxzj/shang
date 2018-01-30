#include "server.h"
#include <sys/uio.h>
#include <math.h>

static void setProtocolError(client *c, int pos);

/* Return the size consumed from the allocator, for the specified SDS string,
 * including internal fragmentation. This function is used in order to compute
 * the client output buffer size. */
// 计算sds保存字符串数据的空间大小
size_t sdsZmallocSize(sds s) {
    // 返回buf[]柔性数组的地址
    void *sh = sdsAllocPtr(s);
    return zmalloc_size(sh);
}

/* Return the amount of memory used by the sds string at object->ptr
 * for a string object. */
// 返回字符串对象所使用的内存数
size_t getStringObjectSdsUsedMemory(robj *o) {
    serverAssertWithInfo(NULL,o,o->type == OBJ_STRING);
    switch(o->encoding) {
    // 返回buf的内存大小
    case OBJ_ENCODING_RAW: return sdsZmallocSize(o->ptr);
    case OBJ_ENCODING_EMBSTR: return zmalloc_size(o)-sizeof(robj);
    default: return 0; /* Just integer encoding for now. */
    }
}

void *dupClientReplyValue(void *o) {
    incrRefCount((robj*)o);
    return o;
}

// 订阅模式下，比较两个字符串对象，以二进制安全的方式进行比较
int listMatchObjects(void *a, void *b) {
    return equalStringObjects(a,b);
}

// 创建一个新的client
client *createClient(int fd) {
    client *c = zmalloc(sizeof(client));    //分配空间

    /* passing -1 as fd it is possible to create a non connected client.
     * This is useful since all the commands needs to be executed
     * in the context of a client. When commands are executed in other
     * contexts (for instance a Lua script) we need a non connected client. */
    // 如果fd为-1，表示创建的是一个无网络连接的伪客户端，用于执行lua脚本的时候。
    // 如果fd不等于-1，表示创建一个有网络连接的客户端
    if (fd != -1) {
        // 设置fd为非阻塞模式
        anetNonBlock(NULL,fd);
        // 禁止使用 Nagle 算法，client向内核递交的每个数据包都会立即发送给server出去，TCP_NODELAY
        anetEnableTcpNoDelay(NULL,fd);
        // 如果开启了tcpkeepalive，则设置 SO_KEEPALIVE
        if (server.tcpkeepalive)
            // 设置tcp连接的keep alive选项
            anetKeepAlive(NULL,fd,server.tcpkeepalive);
        // 创建一个文件事件状态el，且监听读事件，开始接受命令的输入
        if (aeCreateFileEvent(server.el,fd,AE_READABLE,
            readQueryFromClient, c) == AE_ERR)
        {
            close(fd);
            zfree(c);
            return NULL;
        }
    }
    // 默认选0号数据库
    selectDb(c,0);
    // 设置client的ID
    c->id = server.next_client_id++;
    // client的套接字
    c->fd = fd;
    // client的名字
    c->name = NULL;
    // 回复固定(静态)缓冲区的偏移量
    c->bufpos = 0;
    // 输入缓存区
    c->querybuf = sdsempty();
    // 输入缓存区的峰值
    c->querybuf_peak = 0;
    // 请求协议类型，内联或者多条命令，初始化为0
    c->reqtype = 0;
    // 参数个数
    c->argc = 0;
    // 参数列表
    c->argv = NULL;
    // 当前执行的命令和最近一次执行的命令
    c->cmd = c->lastcmd = NULL;
    // 查询缓冲区剩余未读取命令的数量
    c->multibulklen = 0;
    // 读入参数的长度
    c->bulklen = -1;
    // 已发的字节数
    c->sentlen = 0;
    // client的状态
    c->flags = 0;
    // 设置创建client的时间和最后一次互动的时间
    c->ctime = c->lastinteraction = server.unixtime;
    // 认证状态
    c->authenticated = 0;
    // replication复制的状态，初始为无
    c->replstate = REPL_STATE_NONE;
    // 设置从节点的写处理器为ack，是否在slave向master发送ack
    c->repl_put_online_on_ack = 0;
    // replication复制的偏移量
    c->reploff = 0;
    // 通过ack命令接收到的偏移量
    c->repl_ack_off = 0;
    // 通过ack命令接收到的偏移量所用的时间
    c->repl_ack_time = 0;
    // 从节点的端口号
    c->slave_listening_port = 0;
    // 从节点IP地址
    c->slave_ip[0] = '\0';
    // 从节点的功能
    c->slave_capa = SLAVE_CAPA_NONE;
    // 回复链表
    c->reply = listCreate();
    // 回复链表的字节数
    c->reply_bytes = 0;
    // 回复缓冲区的内存大小软限制
    c->obuf_soft_limit_reached_time = 0;
    // 回复链表的释放和复制方法
    listSetFreeMethod(c->reply,decrRefCountVoid);
    listSetDupMethod(c->reply,dupClientReplyValue);
    // 阻塞类型
    c->btype = BLOCKED_NONE;
    // 阻塞超过时间
    c->bpop.timeout = 0;
    // 造成阻塞的键字典
    c->bpop.keys = dictCreate(&setDictType,NULL);
    // 存储解除阻塞的键，用于保存PUSH入元素的键，也就是dstkey
    c->bpop.target = NULL;
    // 阻塞状态
    c->bpop.numreplicas = 0;
    // 要达到的复制偏移量
    c->bpop.reploffset = 0;
    // 全局的复制偏移量
    c->woff = 0;
    // 监控的键
    c->watched_keys = listCreate();
    // 订阅频道
    c->pubsub_channels = dictCreate(&setDictType,NULL);
    // 订阅模式
    c->pubsub_patterns = listCreate();
    // 被缓存的peerid，peerid就是 ip:port
    c->peerid = NULL;
    // 订阅发布模式的释放和比较方法
    listSetFreeMethod(c->pubsub_patterns,decrRefCountVoid);
    listSetMatchMethod(c->pubsub_patterns,listMatchObjects);
    // 将真正的client放在服务器的客户端链表中
    if (fd != -1) listAddNodeTail(server.clients,c);
    // 初始化client的事物状态
    initClientMultiState(c);
    return c;
}//end of createClient()

/*
这个函数每次向客户端发送新数据时会调用，有如下行为：
如果client应该接收到了新数据，函数返回 C_OK，并且将写处理程序设置到数据循环中以便socket可写时将新数据写入
如果client不应该接收到新数据，可能是因为：载入AOF的伪client，主节点或者写处理程序设置失败，函数返回C_ERR
函数可能返回C_OK，在没有安装写处理程序的情况如下：
1.事件处理程序已经已经被安装，因为之前输出缓冲区已经有数据
2.client是一个从节点，但是还没有在线，所以只是想在缓冲区累计写操作，而不是想发送他们
通常每次在回复被创建时调用，在添加数据到client的缓冲区之前。如果函数返回C_ERR，没有数据被追加到输出缓冲区
*/
// 准备一个可写的client
int prepareClientToWrite(client *c) {
    // 如果是要执行lua脚本的伪client，则总是返回C_OK，总是可写的
    if (c->flags & CLIENT_LUA) return C_OK;

    /* CLIENT REPLY OFF / SKIP handling: don't send replies. */
    // 如果client没有开启这条命令的回复功能，则返回C_ERR
    // CLIENT_REPLY_OFF设置为不开启，服务器不会回复client命令
    // CLIENT_REPLY_SKIP设置为跳过该条回复，服务器会跳过这条命令的回复
    if (c->flags & (CLIENT_REPLY_OFF|CLIENT_REPLY_SKIP)) return C_ERR;

    // 如果主节点服务器且没有设置强制回复，返回C_ERR
    if ((c->flags & CLIENT_MASTER) &&
        !(c->flags & CLIENT_MASTER_FORCE_REPLY)) return C_ERR;

    // 如果是载入AOF的伪client，则返回C_ERR
    if (c->fd <= 0) return C_ERR; /* Fake client for AOF loading. */

    // 如果client的回复缓冲区为空，且client还有输出的数据，但是没有设置写处理程序，且
    // replication的状态为关闭状态，或已经将RDB传输完成且不向主节点发送ack
    if (!clientHasPendingReplies(c) &&
        !(c->flags & CLIENT_PENDING_WRITE) &&
        (c->replstate == REPL_STATE_NONE ||
         (c->replstate == SLAVE_STATE_ONLINE && !c->repl_put_online_on_ack)))
    {
        // 将client设置为还有输出的数据，但是没有设置写处理程序
        c->flags |= CLIENT_PENDING_WRITE;
        // 将当前client加入到要写或者安装写处理程序的client链表
        listAddNodeHead(server.clients_pending_write,c);
    }

    /* Authorize the caller to queue in the output buffer of this client. */
    // 授权调用者在这个client的输出缓冲区排队
    return C_OK;
}

/* Create a duplicate of the last object in the reply list when
 * it is not exclusively owned by the reply list. */
// 当回复链表的最后一个对象被其他程序引用，则创建一份复制品
robj *dupLastObjectIfNeeded(list *reply) {
    robj *new, *cur;
    listNode *ln;
    serverAssert(listLength(reply) > 0);
    // 尾节点地址
    ln = listLast(reply);
    // 节点保存的对象
    cur = listNodeValue(ln);
    // 创建一份非共享的对象，替代原有的对象
    if (cur->refcount > 1) {
        new = dupStringObject(cur);
        decrRefCount(cur);
        listNodeValue(ln) = new;
    }
    return listNodeValue(ln);
}



/* -----------------------------------------------------------------------------
 * Low level functions to add more data to output buffers.
 * -------------------------------------------------------------------------- */
// 将字符串s添加到固定回复缓冲区c->buf中
int _addReplyToBuffer(client *c, const char *s, size_t len) {
    // 固定回复缓冲区可用的大小
    size_t available = sizeof(c->buf)-c->bufpos;

    // 如果client即将关闭，则直接成功返回
    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) return C_OK;

    /* If there already are entries in the reply list, we cannot
     * add anything more to the static buffer. */
    // 如果回复链表中有内容，就不能继续添加到固定回复缓冲区中
    if (listLength(c->reply) > 0) return C_ERR;

    /* Check that the buffer has enough space available for this string. */
    // 检查空间大小是否满足
    if (len > available) return C_ERR;

    // 将s拷贝到client的buf中，并更新buf的偏移量
    memcpy(c->buf+c->bufpos,s,len);
    c->bufpos+=len;
    return C_OK;
}//end of _addReplyToBuffer()

// 添加大小到回复链表中
void _addReplyObjectToList(client *c, robj *o) {
    robj *tail;

    // 如果client即将关闭，则直接成功返回
    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) return;

    // 如果链表为空，则将对象追加到链表末尾
    if (listLength(c->reply) == 0) {
        incrRefCount(o);
        listAddNodeTail(c->reply,o);
        // 返回字符串对象所使用的内存数，更新回复链表所占的字节数大小
        c->reply_bytes += getStringObjectSdsUsedMemory(o);
    // 链表不为空
    } else {
        // 获取尾节点的值
        tail = listNodeValue(listLast(c->reply));

        /* Append to this object when possible. */
        // 如果是sds，且最后一个节点的sds大小加上添加对象的sds大小小于16k
        if (tail->ptr != NULL &&
            tail->encoding == OBJ_ENCODING_RAW &&
            sdslen(tail->ptr)+sdslen(o->ptr) <= PROTO_REPLY_CHUNK_BYTES)
        {
            // 从链表字节数中，减去尾节点sds保存字符串数据的空间大小
            c->reply_bytes -= sdsZmallocSize(tail->ptr);
            // 然后创建一个份尾节点的非共享的复制品替代链表原有的
            tail = dupLastObjectIfNeeded(c->reply);
            // 将两者拼接到一起
            tail->ptr = sdscatlen(tail->ptr,o->ptr,sdslen(o->ptr));
            // 重新计算链表字节数
            c->reply_bytes += sdsZmallocSize(tail->ptr);

        // 空间大小超过16k
        } else {
            incrRefCount(o);
            // 添加到链表尾部
            listAddNodeTail(c->reply,o);
            // 重新计算链表字节数
            c->reply_bytes += getStringObjectSdsUsedMemory(o);
        }
    }
    // 检查回复缓冲区的大小是否超过系统限制，如果超过则关闭client
    asyncCloseClientOnOutputBufferLimitReached(c);
}// end of_addReplyObjectToList(client *c, robj *o)

// 添加一个c字符串到回复链表中
void _addReplyStringToList(client *c, const char *s, size_t len) {
    robj *tail;

    // 如果client即将关闭，则释放s，返回
    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) return;

    // 如果链表为空，则将对象追加到链表末尾
    if (listLength(c->reply) == 0) {
        // 将c字符串构建成字符串的追加到链表末尾
        robj *o = createStringObject(s,len);
        // 追加到链表末尾
        listAddNodeTail(c->reply,o);
        // 更新链表字节数
        c->reply_bytes += getStringObjectSdsUsedMemory(o);
    // 链表不为空
    } else {
        // 获取尾节点的值
        tail = listNodeValue(listLast(c->reply));

        /* Append to this object when possible. */
        // 最后一个节点的sds大小加上添加字符串长度大小小于16k
        if (tail->ptr != NULL && tail->encoding == OBJ_ENCODING_RAW &&
            sdslen(tail->ptr)+len <= PROTO_REPLY_CHUNK_BYTES)
        {
            // 将字符串和尾节点的sds拼接起来，更新链表字节数
            c->reply_bytes -= sdsZmallocSize(tail->ptr);
            tail = dupLastObjectIfNeeded(c->reply);
            tail->ptr = sdscatlen(tail->ptr,s,len);
            c->reply_bytes += sdsZmallocSize(tail->ptr);
        // 空间大小超过16k，新创建一个节点，追加到链表尾部
        } else {
            robj *o = createStringObject(s,len);

            listAddNodeTail(c->reply,o);
            c->reply_bytes += getStringObjectSdsUsedMemory(o);
        }
    }
    // 检查回复缓冲区的大小是否超过系统限制，如果超过则关闭client
    asyncCloseClientOnOutputBufferLimitReached(c);
}

/* -----------------------------------------------------------------------------
 * Higher level functions to queue data on the client output buffer.
 * The following functions are the ones that commands implementations will call.
 * -------------------------------------------------------------------------- */
// 添加obj到client的回复缓冲区中
void addReply(client *c, robj *obj) {
    // 准备client为可写的
    if (prepareClientToWrite(c) != C_OK) return;

    /* This is an important place where we can avoid copy-on-write
     * when there is a saving child running, avoiding touching the
     * refcount field of the object if it's not needed.
     *
     * If the encoding is RAW and there is room in the static buffer
     * we'll be able to send the object to the client without
     * messing with its page. */
    // 如果子进程正在执行save操作，尽量不要避免修改对象的引用计数
    // 如果是原生的字符串编码，则添加到固定的回复缓冲区中，
    if (sdsEncodedObject(obj)) {
        // 如果固定的回复缓冲区空间不足够，则添加到回复链表中，可能引起内存分配
        if (_addReplyToBuffer(c,obj->ptr,sdslen(obj->ptr)) != C_OK)
            _addReplyObjectToList(c,obj);
    // 如果是int编码的对象
    } else if (obj->encoding == OBJ_ENCODING_INT) {
        /* youhua: if there is room in the static buffer for 32 bytes
         * (more than the max chars a 64 bit integer can take as string) we
         * avoid decoding the object and go for the lower level approach. */
        // 最优化：如果固定的缓冲区大小等于多于32字节，则将整数转换成字符串，保存在固定的缓冲区buf中
        if (listLength(c->reply) == 0 && (sizeof(c->buf) - c->bufpos) >= 32) {
            char buf[32];
            int len;
            // 转换为字符串
            len = ll2string(buf,sizeof(buf),(long)obj->ptr);
            // 将字符串添加到client的buf中
            if (_addReplyToBuffer(c,buf,len) == C_OK)
                return;
            /* else... continue with the normal code path, but should never
             * happen actually since we verified there is room. */
        }
        // 当前对象是整数，但是长度大于32位，则解码成字符串对象
        obj = getDecodedObject(obj);
        // 添加字符串对象的值到固定的回复buf中
        if (_addReplyToBuffer(c,obj->ptr,sdslen(obj->ptr)) != C_OK)
            // 如果添加失败，则保存到回复链表中
            _addReplyObjectToList(c,obj);
        decrRefCount(obj);
    } else {
        serverPanic("Wrong obj->encoding in addReply()");
    }
}//end of addReply()

// 将c字符串复制到client的回复缓冲区中
void addReplyString(client *c, const char *s, size_t len) {
    if (prepareClientToWrite(c) != C_OK) return;    //准备client为可写的
    if (_addReplyToBuffer(c,s,len) != C_OK)         //将字符串复制到client的回复缓冲区中
        _addReplyStringToList(c,s,len);
}

// 按格式添加一个错误回复
void addReplyErrorLength(client *c, const char *s, size_t len) {
    addReplyString(c,"-ERR ",5);
    addReplyString(c,s,len);
    addReplyString(c,"\r\n",2);
}

// 按格式添加一个错误回复
void addReplyError(client *c, const char *err) {
    addReplyErrorLength(c,err,strlen(err));
}

// 按格式添加多个错误回复
void addReplyErrorFormat(client *c, const char *fmt, ...) {
    size_t l, j;
    va_list ap;
    va_start(ap,fmt);
    sds s = sdscatvprintf(sdsempty(),fmt,ap);
    va_end(ap);
    /* Make sure there are no newlines in the string, otherwise invalid protocol
     * is emitted. */
    l = sdslen(s);
    for (j = 0; j < l; j++) {
        if (s[j] == '\r' || s[j] == '\n') s[j] = ' ';
    }
    addReplyErrorLength(c,s,sdslen(s));
    sdsfree(s);
}

/* -----------------------------------------------------------------------------
 * Low level functions to add more data to output buffers.
 * -------------------------------------------------------------------------- */

/* Return true if the specified client has pending reply buffers to write to
 * the socket. */
// 如果指定的client的回复缓冲区中还有数据，则返回真，表示可以写socket
int clientHasPendingReplies(client *c) {
    return c->bufpos || listLength(c->reply);
}

#define MAX_ACCEPTS_PER_CALL 1000
// TCP连接处理程序，创建一个client的连接状态
static void acceptCommonHandler(int fd, int flags, char *ip) {
    client *c;
    // 创建一个新的client
    if ((c = createClient(fd)) == NULL) {
        serverLog(LL_WARNING,
            "Error registering fd event for the new client: %s (fd=%d)",
            strerror(errno),fd);
        close(fd); /* May be already closed, just ignore errors */
        return;
    }
    // 如果新的client超过server规定的maxclients的限制，那么想新client的fd写入错误信息，关闭该client
    // 先创建client，在进行数量检查，是因为更好的写入错误信息
    if (listLength(server.clients) > server.maxclients) {
        char *err = "-ERR max number of clients reached\r\n";
        /* That's a best effort error message, don't check write errors */
        if (write(c->fd,err,strlen(err)) == -1) {
            /* Nothing to do, Just to avoid the warning... */
        }
        // 更新拒接连接的个数
        server.stat_rejected_conn++;
        freeClient(c);
        return;
    }

    // 如果服务器正在以保护模式运行（默认），且没有设置密码，也没有绑定指定的接口，我们就不接受非回环接口的请求。相反，如果需要，我们会尝试解释用户如何解决问题
    if (server.protected_mode &&
        server.bindaddr_count == 0 &&
        server.requirepass == NULL &&
        !(flags & CLIENT_UNIX_SOCKET) &&
        ip != NULL)
    {
        if (strcmp(ip,"127.0.0.1") && strcmp(ip,"::1")) {
            char *err =
                "-DENIED Redis is running in protected mode because protected "
                "mode is enabled, no bind address was specified, no "
                "authentication password is requested to clients. In this mode "
                "connections are only accepted from the loopback interface. "
                "If you want to connect from external computers to Redis you "
                "may adopt one of the following solutions: "
                "1) Just disable protected mode sending the command "
                "'CONFIG SET protected-mode no' from the loopback interface "
                "by connecting to Redis from the same host the server is "
                "running, however MAKE SURE Redis is not publicly accessible "
                "from internet if you do so. Use CONFIG REWRITE to make this "
                "change permanent. "
                "2) Alternatively you can just disable the protected mode by "
                "editing the Redis configuration file, and setting the protected "
                "mode option to 'no', and then restarting the server. "
                "3) If you started the server manually just for testing, restart "
                "it with the '--protected-mode no' option. "
                "4) Setup a bind address or an authentication password. "
                "NOTE: You only need to do one of the above things in order for "
                "the server to start accepting connections from the outside.\r\n";
            if (write(c->fd,err,strlen(err)) == -1) {
                /* Nothing to do, Just to avoid the warning... */
            }
            // 更新拒接连接的个数
            server.stat_rejected_conn++;
            freeClient(c);
            return;
        }
    }

    // 更新连接的数量
    server.stat_numconnections++;
    // 更新client状态的标志
    c->flags |= flags;
}//end of  acceptCommonHandler()

// 创建一个TCP的连接处理程序
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask) {
    int cport, cfd, max = MAX_ACCEPTS_PER_CALL; //最大一个处理1000次连接
    char cip[NET_IP_STR_LEN];
    UNUSED(el);
    UNUSED(mask);
    UNUSED(privdata);

    while(max--) {
        // accept接受client的连接
        cfd = anetTcpAccept(server.neterr, fd, cip, sizeof(cip), &cport);
        if (cfd == ANET_ERR) {
            if (errno != EWOULDBLOCK)
                serverLog(LL_WARNING,
                    "Accepting client connection: %s", server.neterr);
            return;
        }
        // 打印连接的日志
        serverLog(LL_VERBOSE,"Accepted %s:%d", cip, cport);
        // 创建一个连接状态的client
        acceptCommonHandler(cfd,0,cip);
    }
}

static void freeClientArgv(client *c) {
    int j;
    for (j = 0; j < c->argc; j++)
        decrRefCount(c->argv[j]);
    c->argc = 0;
    c->cmd = NULL;
}


/* Remove the specified client from global lists where the client could
 * be referenced, not including the Pub/Sub channels.
 * This is used by freeClient() and replicationCacheMaster(). */
// 从client所有保存各种client状态的链表中删除指定的client
void unlinkClient(client *c) {
    listNode *ln;

    /* If this is marked as current client unset it. */
    // 如果指定的client被被标记为用于崩溃报告的client，则删除
    if (server.current_client == c) server.current_client = NULL;

    /* Certain operations must be done only if the client has an active socket.
     * If the client was already unlinked or if it's a "fake client" the
     * fd is already set to -1. */
    // 指定的client不是伪client，或不是已经删除的client
    if (c->fd != -1) {
        /* Remove from the list of active clients. */
        // 从client链表中找到地址
        ln = listSearchKey(server.clients,c);
        serverAssert(ln != NULL);
        // 删除当前client的节点
        listDelNode(server.clients,ln);
        /* Unregister async I/O handlers and close the socket. */
        // 从文件事件中删除对该client的fd的监听
        aeDeleteFileEvent(server.el,c->fd,AE_READABLE);
        aeDeleteFileEvent(server.el,c->fd,AE_WRITABLE);
        // 释放文件描述符
        close(c->fd);
        c->fd = -1;
    }

    /* Remove from the list of pending writes if needed. */
    // 如果client还有输出的数据，但是没有设置写处理程序
    if (c->flags & CLIENT_PENDING_WRITE) {
        // 要写或者安装写处理程序的client链表找到当前client
        ln = listSearchKey(server.clients_pending_write,c);
        serverAssert(ln != NULL);
        // 删除当前client的节点
        listDelNode(server.clients_pending_write,ln);
        // 取消标志
        c->flags &= ~CLIENT_PENDING_WRITE;
    }

    /* When client was just unblocked because of a blocking operation,
     * remove it from the list of unblocked clients. */
    // 如果指定的client是非阻塞的
    if (c->flags & CLIENT_UNBLOCKED) {
        // 则从非阻塞的client链表中找到并删除
        ln = listSearchKey(server.unblocked_clients,c);
        serverAssert(ln != NULL);
        listDelNode(server.unblocked_clients,ln);
        // 取消标志
        c->flags &= ~CLIENT_UNBLOCKED;
    }
}//end of unlinkClient()

// 释放client
void freeClient(client *c) {
    listNode *ln;

    //// 如果client是主服务器的，则要缓存client
    //if (server.master && c->flags & CLIENT_MASTER) {
    //    serverLog(LL_WARNING,"Connection with master lost.");
    //    // 如果client不是 即将要关闭的 或 正要关闭的 阻塞或非阻塞的client
    //    if (!(c->flags & (CLIENT_CLOSE_AFTER_REPLY|
    //                      CLIENT_CLOSE_ASAP|
    //                      CLIENT_BLOCKED|
    //                      CLIENT_UNBLOCKED)))
    //    {
    //        // 如果是主节点的client，要缓存client，可以迅速重新启用恢复，不用整体从头建立连接
    //        replicationCacheMaster(c);
    //        return;
    //    }
    //}

    ///* Log link disconnection with slave */
    //// 如果是从服务器的client，且不是执行监控的client
    //if ((c->flags & CLIENT_SLAVE) && !(c->flags & CLIENT_MONITOR)) {
    //    // 更新日志
    //    serverLog(LL_WARNING,"Connection with slave %s lost.",
    //        replicationGetSlaveName(c));
    //}

    /* Free the query buffer */
    // 清空查询缓存
    sdsfree(c->querybuf);
    c->querybuf = NULL;

    ///* Deallocate structures used to block on blocking ops. */
    //// 如果是阻塞的client，则解除阻塞
    //if (c->flags & CLIENT_BLOCKED) unblockClient(c);
    //// 释放造成client阻塞的键
    //dictRelease(c->bpop.keys);

    ///* UNWATCH all the keys */
    //// 清空监视的键
    //unwatchAllKeys(c);
    //listRelease(c->watched_keys);

    ///* Unsubscribe from all the pubsub channels */
    //// 退订所有的频道和模式
    //pubsubUnsubscribeAllChannels(c,0);
    //pubsubUnsubscribeAllPatterns(c,0);
    //dictRelease(c->pubsub_channels);
    //listRelease(c->pubsub_patterns);

    /* Free data structures. */
    // 释放回复链表
    listRelease(c->reply);
    // 释放client的参数列表
    freeClientArgv(c);

    /* Unlink the client: this will close the socket, remove the I/O
     * handlers, and remove references of the client from different
     * places where active clients may be referenced. */
    // 从不同状态的client链表中删除client，会关闭socket，从事件循环中移除对该client的监听
    unlinkClient(c);

    /* Master/slave cleanup Case 1:
     * we lost the connection with a slave. */
    // 如果是从节点的client
    if (c->flags & CLIENT_SLAVE) {
        //// 如果当前服务器的复制状态为：正在发送RDB文件给从节点
        //if (c->replstate == SLAVE_STATE_SEND_BULK) {
        //    // 关闭用于保存主服务器发送RDB文件的文件描述符
        //    if (c->repldbfd != -1) close(c->repldbfd);
        //    // 释放RDB文件的字符串形式的大小
        //    if (c->replpreamble) sdsfree(c->replpreamble);
        //}
        //// 获取保存当前client的链表地址，监控器链表或从节点链表
        //list *l = (c->flags & CLIENT_MONITOR) ? server.monitors : server.slaves;
        //// 取出保存client的节点
        //ln = listSearchKey(l,c);
        //serverAssert(ln != NULL);
        //// 删除该client
        //listDelNode(l,ln);
        ///* We need to remember the time when we started to have zero
        // * attached slaves, as after some time we'll free the replication
        // * backlog. */
        //// 服务器从节点链表为空，要保存当前时间
        //if (c->flags & CLIENT_SLAVE && listLength(server.slaves) == 0)
        //    server.repl_no_slaves_since = server.unixtime;
        //// 重新计算状态良好的从节点服务器的数量
        //refreshGoodSlavesCount();
    }

    /* Master/slave cleanup Case 2:
     * we lost the connection with the master. */
    // 如果是主节点的client，处理从服务器和主服务器的断开
    if (c->flags & CLIENT_MASTER) replicationHandleMasterDisconnection();
    /* If this client was scheduled for async freeing we need to remove it
     * from the queue. */
    // 如果client即将关闭，则从clients_to_close中找到并删除
    if (c->flags & CLIENT_CLOSE_ASAP) {
        ln = listSearchKey(server.clients_to_close,c);
        serverAssert(ln != NULL);
        listDelNode(server.clients_to_close,ln);
    }
    /* Release other dynamically allocated client structure fields,
     * and finally release the client structure itself. */
    // 如果client有名字，则释放
    if (c->name) decrRefCount(c->name);
    // 释放参数列表
    zfree(c->argv);
    // 清除事物状态
    freeClientMultiState(c);
    sdsfree(c->peerid);
    zfree(c);
}//end of freeClient()

/* Schedule a client to free it at a safe time in the serverCron() function.
 * This function is useful when we need to terminate a client but we are in
 * a context where calling freeClient() is not possible, because the client
 * should be valid for the continuation of the flow of the program. */
// 异步释放client
void freeClientAsync(client *c) {
    // 如果是已经即将关闭或者是lua脚本的伪client，则直接返回
    if (c->flags & CLIENT_CLOSE_ASAP || c->flags & CLIENT_LUA) return;
    c->flags |= CLIENT_CLOSE_ASAP;
    // 将client加入到即将关闭的client链表中
    listAddNodeTail(server.clients_to_close,c);
}

/* Write data in output buffers to client. Return C_OK if the client
 * is still valid after the call, C_ERR if it was freed. */
// 将输出缓冲区的数据写给client，如果client被释放则返回C_ERR，没被释放则返回C_OK
int writeToClient(int fd, client *c, int handler_installed) {
    ssize_t nwritten = 0, totwritten = 0;
    size_t objlen;
    size_t objmem;
    robj *o;

    // 如果指定的client的回复缓冲区中还有数据，则返回真，表示可以写socket
    while(clientHasPendingReplies(c)) {
        // 固定缓冲区发送未完成
        if (c->bufpos > 0) {
            // 将缓冲区的数据写到fd中
            nwritten = write(fd,c->buf+c->sentlen,c->bufpos-c->sentlen);
            // 写失败跳出循环
            if (nwritten <= 0) break;
            // 更新发送的数据计数器
            c->sentlen += nwritten;
            totwritten += nwritten;

            /* If the buffer was sent, set bufpos to zero to continue with
             * the remainder of the reply. */
            // 如果发送的数据等于buf的偏移量，表示发送完成
            if ((int)c->sentlen == c->bufpos) {
                // 则将其重置
                c->bufpos = 0;
                c->sentlen = 0;
            }
        // 固定缓冲区发送完成，发送回复链表的内容
        } else {
            // 回复链表的第一条回复对象，和对象值的长度和所占的内存
            o = listNodeValue(listFirst(c->reply));
            objlen = sdslen(o->ptr);
            objmem = getStringObjectSdsUsedMemory(o);

            // 跳过空对象，并删除这个对象
            if (objlen == 0) {
                listDelNode(c->reply,listFirst(c->reply));
                c->reply_bytes -= objmem;
                continue;
            }

            // 将当前节点的值写到fd中
            nwritten = write(fd, ((char*)o->ptr)+c->sentlen,objlen-c->sentlen);
            // 写失败跳出循环
            if (nwritten <= 0) break;
            // 更新发送的数据计数器
            c->sentlen += nwritten;
            totwritten += nwritten;

            /* If we fully sent the object on head go to the next one */
            // 发送完成，则删除该节点，重置发送的数据长度，更新回复链表的总字节数
            if (c->sentlen == objlen) {
                listDelNode(c->reply,listFirst(c->reply));
                c->sentlen = 0;
                c->reply_bytes -= objmem;
            }
        }
        /* Note that we avoid to send more than NET_MAX_WRITES_PER_EVENT
         * bytes, in a single threaded server it's a good idea to serve
         * other clients as well, even if a very large request comes from
         * super fast link that is always able to accept data (in real world
         * scenario think about 'KEYS *' against the loopback interface).
         *
         * However if we are over the maxmemory limit we ignore that and
         * just deliver as much data as it is possible to deliver. */
        // 更新写到网络的字节数
        server.stat_net_output_bytes += totwritten;
        // 如果这次写的总量大于NET_MAX_WRITES_PER_EVENT的限制，则会中断本次的写操作，将处理时间让给其他的client，以免一个非常的回复独占服务器，剩余的数据下次继续在写
        // 但是，如果当服务器的内存数已经超过maxmemory，即使超过最大写NET_MAX_WRITES_PER_EVENT的限制，也会继续执行写入操作，是为了尽快写入给客户端
        if (totwritten > NET_MAX_WRITES_PER_EVENT &&
            (server.maxmemory == 0 ||
             zmalloc_used_memory() < server.maxmemory)) break;
    }
    // 处理写入失败
    if (nwritten == -1) {
        if (errno == EAGAIN) {
            nwritten = 0;
        } else {
            serverLog(LL_VERBOSE,
                "Error writing to client: %s", strerror(errno));
            freeClient(c);
            return C_ERR;
        }
    }
    // 写入成功
    if (totwritten > 0) {
        /* For clients representing masters we don't count sending data
         * as an interaction, since we always send REPLCONF ACK commands
         * that take some time to just fill the socket output buffer.
         * We just rely on data / pings received for timeout detection. */
        // 如果不是主节点服务器，则更新最近和服务器交互的时间
        if (!(c->flags & CLIENT_MASTER)) c->lastinteraction = server.unixtime;
    }
    // 如果指定的client的回复缓冲区中已经没有数据，发送完成
    if (!clientHasPendingReplies(c)) {
        c->sentlen = 0;
        // 删除当前client的可读事件的监听
        if (handler_installed) aeDeleteFileEvent(server.el,c->fd,AE_WRITABLE);

        /* Close connection after entire reply has been sent. */
        // 如果指定了写入按成之后立即关闭的标志，则释放client
        if (c->flags & CLIENT_CLOSE_AFTER_REPLY) {
            freeClient(c);
            return C_ERR;
        }
    }
    return C_OK;
}

/* Write event handler. Just send data to the client. */
// 写事件处理程序，只是发送回复给client
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask) {
    UNUSED(el);
    UNUSED(mask);
    // 发送完数据会删除fd的可读事件
    writeToClient(fd,privdata,1);
}

/* This function is called just before entering the event loop, in the hope
 * we can just write the replies to the client output buffer without any
 * need to use a syscall in order to install the writable event handler,
 * get it called, and so forth. */
// 这个函数是在进入事件循环之前调用的，希望我们只需要将回复写入客户端输出缓冲区，而不需要使用系统调用来安装可写事件处理程序，调用它等等。
int handleClientsWithPendingWrites(void) {
    listIter li;
    listNode *ln;
    // 要写或者安装写处理程序的client链表的长度
    int processed = listLength(server.clients_pending_write);
    // 设置遍历方向
    listRewind(server.clients_pending_write,&li);
    // 遍历链表
    while((ln = listNext(&li))) {
        // 取出当前client
        client *c = listNodeValue(ln);
        // 删除client的 要写或者安装写处理程序 的标志
        c->flags &= ~CLIENT_PENDING_WRITE;
        // 从要写或者安装写处理程序的client链表中删除
        listDelNode(server.clients_pending_write,ln);

        /* Try to write buffers to the client socket. */
        // 将client的回复数据发送给client，但是不会删除fd的可读事件
        if (writeToClient(c->fd,c,0) == C_ERR) continue;

        /* If there is nothing left, do nothing. Otherwise install
         * the write handler. */
        // 如果指定的client的回复缓冲区中还有数据，那么安装写处理程序，否则异步释放client
        if (clientHasPendingReplies(c) &&
            aeCreateFileEvent(server.el, c->fd, AE_WRITABLE,
                sendReplyToClient, c) == AE_ERR)
        {
            freeClientAsync(c);
        }
    }
    // 返回处理的client的个数
    return processed;
}//end of handleClientsWithPendingWrites()



/* resetClient prepare the client to process the next command */
// 重置client准备去处理下一个命令
void resetClient(client *c) {
    redisCommandProc *prevcmd = c->cmd ? c->cmd->proc : NULL;

    // 释放参数列表，重置client
    freeClientArgv(c);
    c->reqtype = 0;
    c->multibulklen = 0;
    c->bulklen = -1;

    /* We clear the ASKING flag as well if we are not inside a MULTI, and
     * if what we just executed is not the ASKING command itself. */
    // 清除CLIENT_ASKING的标志
    if (!(c->flags & CLIENT_MULTI) && prevcmd != askingCommand)
        c->flags &= ~CLIENT_ASKING;

    /* Remove the CLIENT_REPLY_SKIP flag if any so that the reply
     * to the next command will be sent, but set the flag if the command
     * we just processed was "CLIENT REPLY SKIP". */
    // 清理CLIENT_REPLY_SKIP标识，不跳过当前命令的回复
    c->flags &= ~CLIENT_REPLY_SKIP;
    // 为下一条命令设置CLIENT_REPLY_SKIP标志
    if (c->flags & CLIENT_REPLY_SKIP_NEXT) {
        c->flags |= CLIENT_REPLY_SKIP;
        c->flags &= ~CLIENT_REPLY_SKIP_NEXT;
    }
}//end of resetClient()



/* Helper function. Trims query buffer to make the function that processes
 * multi bulk requests idempotent. */
// 修剪查询缓冲区，用来处理多批量请求
static void setProtocolError(client *c, int pos) {
    // 处理日志
    if (server.verbosity <= LL_VERBOSE) {
        // 将client的所有信息转换为sds，打印到日志里
        sds client = catClientInfoString(sdsempty(),c);
        serverLog(LL_VERBOSE,
            "Protocol error from client: %s", client);
        sdsfree(client);
    }
    // 设置发送回复后立即关闭client
    c->flags |= CLIENT_CLOSE_AFTER_REPLY;
    // 修剪查询缓冲区
    sdsrange(c->querybuf,pos,-1);
}

// 将client的querybuf中的协议内容转换为client的参数列表中的对象
int processMultibulkBuffer(client *c) {
    char *newline = NULL;
    int pos = 0, ok;
    long long ll;

    // 参数列表中命令数量为0，因此先分配空间
    if (c->multibulklen == 0) {
        /* The client should have been reset */
        serverAssertWithInfo(c,NULL,c->argc == 0);

        /* Multi bulk length cannot be read without a \r\n */
        // 查询第一个换行符
        newline = strchr(c->querybuf,'\r');
        // 没有找到\r\n，表示不符合协议，返回错误
        if (newline == NULL) {
            if (sdslen(c->querybuf) > PROTO_INLINE_MAX_SIZE) {
                addReplyError(c,"Protocol error: too big mbulk count string");
                setProtocolError(c,0);
            }
            return C_ERR;
        }

        /* Buffer should also contain \n */
        // 检查格式
        if (newline-(c->querybuf) > ((signed)sdslen(c->querybuf)-2))
            return C_ERR;

        /* We know for sure there is a whole line since newline != NULL,
         * so go ahead and find out the multi bulk length. */
        // 保证第一个字符为'*'
        serverAssertWithInfo(c,NULL,c->querybuf[0] == '*');
        // 将'*'之后的数字转换为整数。*3\r\n
        ok = string2ll(c->querybuf+1,newline-(c->querybuf+1),&ll);
        if (!ok || ll > 1024*1024) {
            addReplyError(c,"Protocol error: invalid multibulk length");
            setProtocolError(c,pos);
            return C_ERR;
        }

        // 指向"*3\r\n"的"\r\n"之后的位置
        pos = (newline-c->querybuf)+2;
        // 空白命令，则将之前的删除，保留未阅读的部分
        if (ll <= 0) {
            sdsrange(c->querybuf,pos,-1);
            return C_OK;
        }

        // 参数数量
        c->multibulklen = ll;

        /* Setup argv array on client structure */
        // 分配client参数列表的空间
        if (c->argv) zfree(c->argv);
        c->argv = zmalloc(sizeof(robj*)*c->multibulklen);
    }

    serverAssertWithInfo(c,NULL,c->multibulklen > 0);
    // 读入multibulklen个参数，并创建对象保存在参数列表中
    while(c->multibulklen) {
        /* Read bulk length if unknown */
        // 读入参数的长度
        if (c->bulklen == -1) {
            // 找到换行符，确保"\r\n"存在
            newline = strchr(c->querybuf+pos,'\r');
            if (newline == NULL) {
                if (sdslen(c->querybuf) > PROTO_INLINE_MAX_SIZE) {
                    addReplyError(c,
                        "Protocol error: too big bulk count string");
                    setProtocolError(c,0);
                    return C_ERR;
                }
                break;
            }

            /* Buffer should also contain \n */
            // 检查格式
            if (newline-(c->querybuf) > ((signed)sdslen(c->querybuf)-2))
                break;

            // $3\r\nSET\r\n...，确保是'$'字符，保证格式
            if (c->querybuf[pos] != '$') {
                addReplyErrorFormat(c,
                    "Protocol error: expected '$', got '%c'",
                    c->querybuf[pos]);
                setProtocolError(c,pos);
                return C_ERR;
            }

            // 将参数长度保存到ll。
            ok = string2ll(c->querybuf+pos+1,newline-(c->querybuf+pos+1),&ll);
            if (!ok || ll < 0 || ll > 512*1024*1024) {
                addReplyError(c,"Protocol error: invalid bulk length");
                setProtocolError(c,pos);
                return C_ERR;
            }

            // 定位第一个参数的位置，也就是SET的S
            pos += newline-(c->querybuf+pos)+2;
            // 参数长度太长，进行优化
            if (ll >= PROTO_MBULK_BIG_ARG) {
                size_t qblen;

                /* If we are going to read a large object from network
                 * try to make it likely that it will start at c->querybuf
                 * boundary so that we can optimize object creation
                 * avoiding a large copy of data. */
                // 如果我们要从网络中读取一个大的对象，尝试使它可能从c-> querybuf边界开始，以便我们可以优化对象创建，避免大量的数据副本
                // 保存未读取的部分
                sdsrange(c->querybuf,pos,-1);
                // 重置偏移量
                pos = 0;
                // 获取querybuf中已使用的长度
                qblen = sdslen(c->querybuf);
                /* Hint the sds library about the amount of bytes this string is
                 * going to contain. */
                // 扩展querybuf的大小
                if (qblen < (size_t)ll+2)
                    c->querybuf = sdsMakeRoomFor(c->querybuf,ll+2-qblen);
            }
            // 保存参数的长度
            c->bulklen = ll;
        }

        /* Read bulk argument */
        // 因为只读了multibulklen字节的数据，读到的数据不够，则直接跳出循环，执行processInputBuffer()函数循环读取
        if (sdslen(c->querybuf)-pos < (unsigned)(c->bulklen+2)) {
            /* Not enough data (+2 == trailing \r\n) */
            break;
        // 为参数创建了对象
        } else {
            /* Optimization: if the buffer contains JUST our bulk element
             * instead of creating a new object by *copying* the sds we
             * just use the current sds string. */
            // 如果读入的长度大于32k
            if (pos == 0 &&
                c->bulklen >= PROTO_MBULK_BIG_ARG &&
                (signed) sdslen(c->querybuf) == c->bulklen+2)
            {
                c->argv[c->argc++] = createObject(OBJ_STRING,c->querybuf);
                // 跳过换行
                sdsIncrLen(c->querybuf,-2); /* remove CRLF */
                /* Assume that if we saw a fat argument we'll see another one
                 * likely... */
                // 设置一个新长度
                c->querybuf = sdsnewlen(NULL,c->bulklen+2);
                sdsclear(c->querybuf);
                pos = 0;
            // 创建对象保存在client的参数列表中
            } else {
                c->argv[c->argc++] =
                    createStringObject(c->querybuf+pos,c->bulklen);
                pos += c->bulklen+2;
            }
            // 清空命令内容的长度
            c->bulklen = -1;
            // 未读取命令参数的数量，读取一个，该值减1
            c->multibulklen--;
        }
    }

    /* Trim to pos */
    // 删除已经读取的，保留未读取的
    if (pos) sdsrange(c->querybuf,pos,-1);

    /* We're done when c->multibulk == 0 */
    // 命令的参数全部被读取完
    if (c->multibulklen == 0) return C_OK;

    /* Still not read to process the command */
    return C_ERR;
}

// 处理client输入的命令内容
void processInputBuffer(client *c) {
    server.current_client = c;
    /* Keep processing while there is something in the input buffer */
    // 一直读输入缓冲区的内容
    while(sdslen(c->querybuf)) {
        /* Return if clients are paused. */
        // 如果处于暂停状态，直接返回
        if (!(c->flags & CLIENT_SLAVE) && clientsArePaused()) break;

        /* Immediately abort if the client is in the middle of something. */
        // 如果client处于被阻塞状态，直接返回
        if (c->flags & CLIENT_BLOCKED) break;

        /* CLIENT_CLOSE_AFTER_REPLY closes the connection once the reply is
         * written to the client. Make sure to not let the reply grow after
         * this flag has been set (i.e. don't process more commands).
         *
         * The same applies for clients we want to terminate ASAP. */
        // 如果client处于关闭状态，则直接返回
        if (c->flags & (CLIENT_CLOSE_AFTER_REPLY|CLIENT_CLOSE_ASAP)) break;

        /* Determine request type when unknown. */
        // 如果是未知的请求类型，则判定请求类型
        if (!c->reqtype) {
            // 如果是"*"开头，则是多条请求，是client发来的
            if (c->querybuf[0] == '*') {
                c->reqtype = PROTO_REQ_MULTIBULK;
            // 否则就是内联请求，是Telnet发来的
            } else {
                c->reqtype = PROTO_REQ_INLINE;
            }
        }

        // 如果是内联请求
        if (c->reqtype == PROTO_REQ_INLINE) {
            // 处理Telnet发来的内联命令，并创建成对象，保存在client的参数列表中
            //if (processInlineBuffer(c) != C_OK) break;//TODO
        // 如果是多条请求
        } else if (c->reqtype == PROTO_REQ_MULTIBULK) {
            // 将client的querybuf中的协议内容转换为client的参数列表中的对象
            if (processMultibulkBuffer(c) != C_OK) break;
        } else {
            serverPanic("Unknown request type");
        }

        /* Multibulk processing could see a <= 0 length. */
        // 如果参数为0，则重置client
        if (c->argc == 0) {
            resetClient(c);
        } else {
            /* Only reset the client when the command was executed. */
            // 执行命令成功后重置client
            if (processCommand(c) == C_OK)
                resetClient(c);
            /* freeMemoryIfNeeded may flush slave output buffers. This may result
             * into a slave, that may be the active client, to be freed. */
            if (server.current_client == NULL) break;
        }
    }
    // 执行成功，则将用于崩溃报告的client设置为NULL
    server.current_client = NULL;
}//end of processInputBuffer()



/* This function returns the number of bytes that Redis is virtually
 * using to store the reply still not read by the client.
 * It is "virtual" since the reply output list may contain objects that
 * are shared and are not really using additional memory.
 *
 * The function returns the total sum of the length of all the objects
 * stored in the output list, plus the memory used to allocate every
 * list node. The static reply buffer is not taken into account since it
 * is allocated anyway.
 *
 * Note: this function is very fast so can be called as many time as
 * the caller wishes. The main usage of this function currently is
 * enforcing the client output length limits. */
// 返回输出缓冲区的大小，不包含静态的固定回复缓冲区，因为他总被分配
unsigned long getClientOutputBufferMemoryUsage(client *c) {
    // 回复链表节点和值对象的空间
    unsigned long list_item_size = sizeof(listNode)+sizeof(robj);

    // 所有的回复数据的字节数，加上保存这些数据的数据结构所使用的空间
    return c->reply_bytes + (list_item_size*listLength(c->reply));
}

/* Get the class of a client, used in order to enforce limits to different
 * classes of clients.
 *
 * The function will return one of the following:
 * CLIENT_TYPE_NORMAL -> Normal client
 * CLIENT_TYPE_SLAVE  -> Slave or client executing MONITOR command
 * CLIENT_TYPE_PUBSUB -> Client subscribed to Pub/Sub channels
 * CLIENT_TYPE_MASTER -> The client representing our replication master.
 */
// 获取client的类型，用于不同类型的client应用不同的限制
// CLIENT_TYPE_NORMAL 普通client
// CLIENT_TYPE_SLAVE 从节点或执行 MONITOR 命令的client
// CLIENT_TYPE_PUBSUB 进行订阅操作的client
// CLIENT_TYPE_MASTER 执行复制主节点服务器操作的client
int getClientType(client *c) {
    if (c->flags & CLIENT_MASTER) return CLIENT_TYPE_MASTER;
    if ((c->flags & CLIENT_SLAVE) && !(c->flags & CLIENT_MONITOR))
        return CLIENT_TYPE_SLAVE;
    if (c->flags & CLIENT_PUBSUB) return CLIENT_TYPE_PUBSUB;
    return CLIENT_TYPE_NORMAL;
}

// 读取client的输入缓冲区的内容
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask) {
	client *c = (client*) privdata;
	int nread, readlen;
	size_t qblen;
	UNUSED(el);
	UNUSED(mask);
	// 读入的长度，默认16MB
	readlen = PROTO_IOBUF_LEN;
	// 如果是多条请求，根据请求的大小，设置读入的长度readlen
	if (c->reqtype == PROTO_REQ_MULTIBULK && c->multibulklen && c->bulklen != -1
			&& c->bulklen >= PROTO_MBULK_BIG_ARG)
	{
		int remaining = (unsigned)(c->bulklen+2)-sdslen(c->querybuf);
		if (remaining < readlen) readlen = remaining;
	}
	// 输入缓冲区的长度
	qblen = sdslen(c->querybuf);
	// 更新缓冲区的峰值
	if (c->querybuf_peak < qblen) c->querybuf_peak = qblen;
	// 扩展缓冲区的大小
	c->querybuf = sdsMakeRoomFor(c->querybuf, readlen);
	// 将client发来的命令，读入到输入缓冲区中
	nread = read(fd, c->querybuf+qblen, readlen);
	// 读操作出错
	if (nread == -1) {
		if (errno == EAGAIN) {
			return;
		} else {
			serverLog(LL_VERBOSE, "Reading from client: %s",strerror(errno));
			freeClient(c);
			return;
		}
		// 读操作完成
	} else if (nread == 0) {
		serverLog(LL_VERBOSE, "Client closed connection");
		freeClient(c);
		return;
	}
	// 更新输入缓冲区的已用大小和未用大小。
	sdsIncrLen(c->querybuf,nread);
	// 设置最后一次服务器和client交互的时间
	c->lastinteraction = server.unixtime;
	// 如果是主节点，则更新复制操作的偏移量
	if (c->flags & CLIENT_MASTER) c->reploff += nread;
	// 更新从网络输入的字节数
	server.stat_net_input_bytes += nread;
	// 如果输入缓冲区长度超过服务器设置的最大缓冲区长度
	if (sdslen(c->querybuf) > server.client_max_querybuf_len) {
		// 将client信息转换为sds
		sds ci = catClientInfoString(sdsempty(),c), bytes = sdsempty();
		// 输入缓冲区保存在bytes中
		bytes = sdscatrepr(bytes,c->querybuf,64);
		// 打印到日志
		serverLog(LL_WARNING,"Closing client that reached max query buffer length: %s (qbuf initial bytes: %s)", ci, bytes);
		// 释放空间
		sdsfree(ci);
		sdsfree(bytes);
		freeClient(c);
		return;
	}
	// 处理client输入的命令内容
	processInputBuffer(c);
}//end of readQueryFromClient()


// 生成client的Peer ID
void genClientPeerId(client *client, char *peerid,
                            size_t peerid_len) {
    // 本机socket的client
    if (client->flags & CLIENT_UNIX_SOCKET) {
        /* Unix socket client. */
        snprintf(peerid,peerid_len,"%s:0",server.unixsocket);
    } else {
        // TCP连接的client
        /* TCP client. */
        anetFormatPeer(client->fd,peerid,peerid_len);
    }
}

// 获取client的Peer ID
char *getClientPeerId(client *c) {
    char peerid[NET_PEER_ID_LEN];

    // 如果client没有，则生成一个
    if (c->peerid == NULL) {
        genClientPeerId(c,peerid,sizeof(peerid));
        c->peerid = sdsnew(peerid);
    }
    return c->peerid;
}

/* Concatenate a string representing the state of a client in an human
 * readable format, into the sds string 's'. */
// 获取client的每种信息，并以sds形式返回
sds catClientInfoString(sds s, client *client) {
    char flags[16], events[3], *p;
    int emask;

    p = flags;
    if (client->flags & CLIENT_SLAVE) {
        if (client->flags & CLIENT_MONITOR)
            *p++ = 'O';
        else
            *p++ = 'S';
    }
    if (client->flags & CLIENT_MASTER) *p++ = 'M';
    if (client->flags & CLIENT_MULTI) *p++ = 'x';
    if (client->flags & CLIENT_BLOCKED) *p++ = 'b';
    if (client->flags & CLIENT_DIRTY_CAS) *p++ = 'd';
    if (client->flags & CLIENT_CLOSE_AFTER_REPLY) *p++ = 'c';
    if (client->flags & CLIENT_UNBLOCKED) *p++ = 'u';
    if (client->flags & CLIENT_CLOSE_ASAP) *p++ = 'A';
    if (client->flags & CLIENT_UNIX_SOCKET) *p++ = 'U';
    if (client->flags & CLIENT_READONLY) *p++ = 'r';
    if (p == flags) *p++ = 'N';
    *p++ = '\0';

    emask = client->fd == -1 ? 0 : aeGetFileEvents(server.el,client->fd);
    p = events;
    if (emask & AE_READABLE) *p++ = 'r';
    if (emask & AE_WRITABLE) *p++ = 'w';
    *p = '\0';
    return sdscatfmt(s,
        "id=%U addr=%s fd=%i name=%s age=%I idle=%I flags=%s db=%i sub=%i psub=%i multi=%i qbuf=%U qbuf-free=%U obl=%U oll=%U omem=%U events=%s cmd=%s",
        (unsigned long long) client->id,
        getClientPeerId(client),
        client->fd,
        client->name ? (char*)client->name->ptr : "",
        (long long)(server.unixtime - client->ctime),
        (long long)(server.unixtime - client->lastinteraction),
        flags,
        client->db->id,
        (int) dictSize(client->pubsub_channels),
        (int) listLength(client->pubsub_patterns),
        (client->flags & CLIENT_MULTI) ? client->mstate.count : -1,
        (unsigned long long) sdslen(client->querybuf),
        (unsigned long long) sdsavail(client->querybuf),
        (unsigned long long) client->bufpos,
        (unsigned long long) listLength(client->reply),
        (unsigned long long) getClientOutputBufferMemoryUsage(client),
        events,
        client->lastcmd ? client->lastcmd->name : "NULL");
}//end of catClientInfoString()

// 检查client的输出缓冲区是否到达限制，到达限制则进行标记
int checkClientOutputBufferLimits(client *c) {
    int soft = 0, hard = 0, class;
    // 获取输出缓冲区的大小，不包含静态的固定回复缓冲区
    unsigned long used_mem = getClientOutputBufferMemoryUsage(c);

    // 获取client的类型
    class = getClientType(c);
    /* For the purpose of output buffer limiting, masters are handled
     * like normal clients. */
    // 将主节点服务器当做普通的类型的client看待
    if (class == CLIENT_TYPE_MASTER) class = CLIENT_TYPE_NORMAL;

    // 检查硬限制
    if (server.client_obuf_limits[class].hard_limit_bytes &&
        used_mem >= server.client_obuf_limits[class].hard_limit_bytes)
        hard = 1;
    // 检查软限制
    if (server.client_obuf_limits[class].soft_limit_bytes &&
        used_mem >= server.client_obuf_limits[class].soft_limit_bytes)
        soft = 1;

    /* We need to check if the soft limit is reached continuously for the
     * specified amount of seconds. */
    // 如果达到软限制
    if (soft) {
        // 第一次达到软限制，设置到达软限制的时间
        if (c->obuf_soft_limit_reached_time == 0) {
            c->obuf_soft_limit_reached_time = server.unixtime;
            soft = 0; /* First time we see the soft limit reached */
        // 之后到达软限制
        } else {
            // 计算软限制之间的时间
            time_t elapsed = server.unixtime - c->obuf_soft_limit_reached_time;

            // 小于设置的软限制时间，则取消标志，可以在规定的时间内暂时超过软限制
            if (elapsed <=
                server.client_obuf_limits[class].soft_limit_seconds) {
                soft = 0; /* The client still did not reached the max number of
                             seconds for the soft limit to be considered
                             reached. */
            }
        }
    // 没有到达软限制，则清空达到软限制的时间
    } else {
        c->obuf_soft_limit_reached_time = 0;
    }
    // 返回是否达到的限制
    return soft || hard;
}//end of checkClientOutputBufferLimits()

// 如果client达到缓冲区的限制，则异步关闭client，防止底层函数正在向client的输出缓冲区写数据
void asyncCloseClientOnOutputBufferLimitReached(client *c) {
	serverAssert(c->reply_bytes < SIZE_MAX-(1024*64));
	// 已经设置 CLIENT_CLOSE_ASAP 标志则直接返回
	if (c->reply_bytes == 0 || c->flags & CLIENT_CLOSE_ASAP) return;
	// 检查client是否达到缓冲区的限制
	if (checkClientOutputBufferLimits(c)) {
		// 将client的信息保存到sds中
		sds client = catClientInfoString(sdsempty(),c);
		// 异步关闭client
		freeClientAsync(c);
		serverLog(LL_WARNING,"Client %s scheduled to be closed ASAP for overcoming of output buffer limits.", client);
		sdsfree(client);
	}
}

/* Return non-zero if clients are currently paused. As a side effect the
 * function checks if the pause time was reached and clear it. */
int clientsArePaused(void) {
	if (server.clients_paused &&
			server.clients_pause_end_time < server.mstime)
	{
		listNode *ln;
		listIter li;
		client *c;

		server.clients_paused = 0;

		/* Put all the clients in the unblocked clients queue in order to
		 * force the re-processing of the input buffer if any. */
		listRewind(server.clients,&li);
		while ((ln = listNext(&li)) != NULL) {
			c = listNodeValue(ln);

			/* Don't touch slaves and blocked clients. The latter pending
			 * requests be processed when unblocked. */
			if (c->flags & (CLIENT_SLAVE|CLIENT_BLOCKED)) continue;
			c->flags |= CLIENT_UNBLOCKED;
			listAddNodeTail(server.unblocked_clients,c);
		}
	}
	return server.clients_paused;
}


