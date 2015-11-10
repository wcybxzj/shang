#ifndef CLIENT_H__
#define CLIENT_H__


//- 是读取标准输入的内容
#define DEFAULT_PLAYERCMD "/usr/bin/mpg123 -"
//#define DEFAULT_PLAYERCMD "/usr/bin/mpg123 - > /dev/null"

struct client_conf_st{
	char *rcvport;
	char  *mgroup;
	char  *player_cmd;
	char  *ifname;
};

extern struct client_conf_st client_conf;

#endif
