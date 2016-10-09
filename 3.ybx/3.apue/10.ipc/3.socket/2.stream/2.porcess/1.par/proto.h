#ifndef PROTO_H__
#define PROTO_H__

#define SERVERPORT "1989"

//协议约定双方对话的格式
//如果客户端是终端可以 %lld\n
//如果客户端是web %lld\r\n
//我这里客户端是终端没加换行
#define FMT_STAMP "%lld\n"

#endif
