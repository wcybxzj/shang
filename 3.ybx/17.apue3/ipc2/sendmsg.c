#include "apue.h"
#include <sys/msg.h>

#define MAXMSZ 512

struct mymesg {
	long mtype;
	char mtext[MAXMSZ];
};


int main(int argc, const char *argv[])
{
	key_t key;
	int qid, ret;
	struct mymesg m;

	if (argc!=3) {
		fprintf(stderr, "usage sendmsg KEY message\n");
		exit(1);
	}

	key =strtol(argv[1], NULL, 0);
	qid = msgget(key, 0);
	if (qid<0) {
		err_sys("cant open queue ,key %s", argv[1]);
	}

	memset(&m, 0,sizeof(m));
	strncpy(m.mtext, argv[2], MAXMSZ-1);
	m.mtype = 1;

	ret = msgsnd(qid, &m, sizeof(m)-sizeof(long),0);
	if (ret<0) {
		err_sys("msgsnd error");
	}
	return 0;
}
