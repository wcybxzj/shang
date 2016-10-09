#ifndef PROTO_H__
#define PROTO_H__


#define KEYPATH "/etc/resolv.conf"
#define KEYPROJ 'a'

#define NAMESIZE 32

#define  ONE 1
#define  TWO 2


struct msg_st{
	long mtype;
	char name[NAMESIZE];
	int math;
	int chinese;
};
#endif
