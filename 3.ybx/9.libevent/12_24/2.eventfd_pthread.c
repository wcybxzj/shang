#include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>             /* Definition of uint64_t */
#include <signal.h>

struct arg {
	int efd;
	int argc;
	char **argv;
} ;

void* func(void *p)
{
	struct arg *p1 = (struct arg* )p;
	int j;
	uint64_t u;
	ssize_t s;
	for (j = 1; j < p1->argc; j++) {
		printf("Child writing %s to efd\n", p1->argv[j]);
		u = strtoull(p1->argv[j], NULL, 0);
		s = write(p1->efd, &u, sizeof(uint64_t));
		if (s != sizeof(uint64_t))
			pthread_exit(NULL);
	}
	printf("Child completed write loop\n");
}

/*
./2.eventfd_pthread 1 2 4 7 14
Child writing 1 to efd
Child writing 2 to efd
Child writing 4 to efd
Child writing 7 to efd
Child writing 14 to efd
Child completed write loop
Parent about to read
Parent read 28 (0x1c) from efd
*/
//多线程和多进程效果完全一样
int
main(int argc, char *argv[])
{
	int efd, j;
	uint64_t u;
	ssize_t s;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	efd = eventfd(0, 0);
	if (efd == -1){
		perror("eventfd");
		exit(1);
	}

	struct arg var;
	var.efd = efd;
	var.argc = argc;
	var.argv = argv;

	pthread_t tid;
	pthread_create(&tid, NULL, func, &var);

	sleep(2);
	printf("Parent about to read\n");
	s = read(efd, &u, sizeof(uint64_t));
	if (s != sizeof(uint64_t)){
		perror("read");
		exit(1);
	}

	printf("Parent read %llu (0x%llx) from efd\n",
			(unsigned long long) u, (unsigned long long) u);

}

