#ifndef PROTO_H__
#define PRORO_H__

#include <site_type.h>

//可变
#define DEFAULT_MGROUP "224.2.2.2"
#define DEFAULT_RCVPORT "1989"
#define DEFAULT_EHT "eth0"

// 频道数
#define CHNNR 100

// 节目单号
#define LISTCHNID 0
// 最小频道号
#define MINCHNID  1
// 最大频道号 
#define MAXCHNID (MINCHNID + CHNNR-1)

//udp推荐512字节可以到65536字节, ip头部20-60字节
#define MSG_CHANNEL_MAX (65536-20-8) 
#define MAX_DATA (MSG_CHANNEL_MAX - sizeof(chid_t))

#define MSG_LIST_MAX (65536-20-8)
#define MAX_ENTRY (MSG_LIST_MAX-sizeof(chnid_t))

struct msg_channel_st{
	chnid_t chnid;//频道号 between [MINCHNID, MAXCHNID]
	uint8_t data[1];//无符号8位整形数,以字节为单位
}__attribute__((packed));

struct msg_listentry_st{
	chnid_t chnid;
	uint16_t len;
	uint8_t desc[1];
}__attribute__((packed));

struct msg_list_st{
	chnid_t chnid;//节目单号 LISTCHNID
	struct msg_listentry_st entry[1];
}__attribute__((packed));

#endif
