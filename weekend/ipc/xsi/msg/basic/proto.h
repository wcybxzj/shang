#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH			"/etc/hosts"
#define KEYPROJ			'a'

#define NAMESIZE		32

#define MSGTYPE			1

struct msg_st
{
	long mtype;
	char name[NAMESIZE];
	int math;
	int chinese;
};


#endif


