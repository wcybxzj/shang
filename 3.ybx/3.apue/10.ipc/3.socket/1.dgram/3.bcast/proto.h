#ifndef PROTO
#define PROTO

#define RCVPORT "1989"


//8是udp头 20是ip头
#define MSGMAX (512-8-20)
///8是sizeof(math)+sizeof(chinese)
#define NAMESIZE (MSGMAX-8)

struct msg_st{
	uint32_t math;//4
	uint32_t chinese;//4
	uint8_t name[1];
}__attribute__((packed));

#endif
