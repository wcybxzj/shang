#ifndef PROTO_H__
#define PROTO_H__

#define KEYPATH		"/etc/hosts"
#define KEYPROJ		'A'

#define PATHSIZE	1024
#define DATASIZE	1024

enum
{
	STATE_RUNNING=1,
	STATE_CANCELED,
	STATE_OVER
};

enum
{
	MSG_PATH=1,
	MSG_DATA,
	MSG_ERR,
	MSG_EOT
};

enum{
	S_STATE_READ_PATH = 1,
	S_STATE_READ_DATA,
	S_STATE_WRITE_DATA,
	C_STATE_WRITE_PATH,
	C_STATE_READ_DATA,
	C_STATE_WRITE_PATH,
	C_STATE_OUTPUT,
	STATE_EX,
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

union msg_s2c_un
{
	long mtype;	
	msg_data_t datamsg;
	msg_err_t errmsg;
	msg_eot_t eotmsg;
};

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




