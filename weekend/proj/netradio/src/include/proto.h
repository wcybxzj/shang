#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>

#define DEFAULT_MGROUP			"224.2.2.2"
#define DEFAULT_RCVPORT			"1989"

#define NR_CHN					200
#define MINCHNID				1
#define MAXCHNID				(MINCHNID+NR_CHN-1)

#define CHNID_LIST				0

typedef uint8_t 	chnid_t;

#define MAX_MSG		(65536-20-8)
#define MAX_DATA	(MAX_MSG-sizeof(uint8_t))
#define MAX_ENTRY	(MAX_MSG-sizeof(uint8_t))

struct msg_list_entry_st
{
	chnid_t chnid;      /* must between [MINCHNID,MAXCHNID] */
	uint8_t desc[1];
}__attribute__((packed));

struct msg_list_st
{
	chnid_t chnid;      /* must be CHNID_LIST */
	struct msg_list_entry_st entry[1];
}__attribute__((packed));

struct msg_channel_st
{
	chnid_t chnid;		/* must between [MINCHNID,MAXCHNID] */	
//	uint32_t data_len;
	uint8_t data[1];		
}__attribute__((packed));



#endif


