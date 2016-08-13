#ifndef RELAYER_H__
#define RELAYER_H__

#define REL_JOBMAX 10000

enum{
	STATE_RUNNING=1,
	STATE_CANCELED,
	STATE_OVER
};

struct rel_stat_st{
	int fd1, fd2;
	int state;
	int64_t count12, count21;
};

int rel_addjob(int fd1, int fd2);
int rel_statejob(int id, struct rel_stat_st *);

#endif
