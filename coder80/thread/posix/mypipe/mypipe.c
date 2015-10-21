

struct mypipe_st
{
	char data[PIPESIZE];
	int head;
	int tail;
	int datasize;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};





