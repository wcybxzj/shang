#ifndef SERVER_CONF_H__
#define SERVER_CONF_H__

#define DEFAULT_MEDIADIR "~/media/"

enum{
	RUN_DAEMON = 1,
	RUN_FOREGROUND
};

struct server_conf_st{
	char *rcvport;
	char *mgroup;
	char *media_dir;
	char run_mode;
	char *ifname;
};

extern struct server_conf_st server_st;

#endif
