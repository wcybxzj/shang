#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>


#define DEFAULT_MGROUP		"224.2.2.3"
#define DEFAULT_RCVPORT		"1989"

#define NR_CHN		200
#define CHNID_LIST	0
#define MINCHNID	1
#define MAXCHNID	(MINCHNID+NR_CHN-1)

typedef uint8_t 	chnid_t;

#define MAXMSG		(65536-20-8)
#define MAXDATA		(MAXMSG-sizeof(uint8_t))
#define MAXENTRY	(MAXMSG-sizeof(uint8_t))

struct msg_chnnal_st
{
	chnid_t chnid;			/* [MINCHNID,MAXCHNID] */
	uint8_t data[1];
	int 	sd;				//fd for socket
}__attribute__((packed));

struct list_entry_st
{
	chnid_t chnid;          /* [MINCHNID,MAXCHNID] */
	uint8_t desc[1];
}__attribute__((packed));

struct msg_list_st
{
	chnid_t chnid;          /* CHNID_LIST */
	struct list_entry_st entry[1];
}__attribute__((packed));

#endif


