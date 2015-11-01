#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'A'

#define PATHSIZE	1024
#define DATASIZE	1024

enum
{
	MSG_PATH=1,
	MSG_DATA,
	MSG_ERR,
	MSG_EOT
};

enum {
	STATE_RCV = 1,
	STATE_SND,
	STATE_Ex,
	STATE_T	
};

typedef struct msg_path_st
{
	long mtype;					/*must be MSG_PATH*/
	char path[PATHSIZE];		/*带尾0的串*/
}msg_path_t;

typedef struct msg_data_st
{
	long mtype;					/*must be MSG_DATA*/
	char data[DATASIZE];
	int datasize;
}msg_data_t;

typedef struct msg_err_st
{
	long mtype;					/*must be MSG_ERR*/
	int errno_;
}msg_err_t;

typedef struct msg_eot_st
{
	long mtype;					/*must be MSG_EOT*/
}msg_eot_t;

typedef union msg_s2c_un
{
	long mtype;	
	msg_data_t datamsg;
	msg_err_t errmsg;
	msg_eot_t eotmsg;
}msg_s2c_u;

/*
struct msg_s2c_st
{
	long mtype;
	union msg_s2c_un
	{
		msg_data_t datamsg;
		msg_err_t errmsg;
		msg_eot_t eotmsg;
	}msg;
}
*/

#endif




