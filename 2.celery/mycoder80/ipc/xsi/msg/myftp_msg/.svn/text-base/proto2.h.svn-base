#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'A'

#define PATHSIZE	1024
#define DATASIZE	1024

enum
{
	MSG_PATH=1,
	MSG_S2C
};

typedef struct msg_path_st
{
	long mtype;					/*must be MSG_PATH*/
	char path[PATHSIZE];		/*带尾0的串*/
}msg_path_t;

typedef struct msg_s2c_st
{
	long mtype;					/*must be MSG_S2C*/
	int datasize;
/*
 *	datasize >  0		datamsg;
 *			 == 0       eotmsg;
 *			 <  0       -errno;
 * */
	char data[DATASIZE];
}msg_s2c_t;


#endif




