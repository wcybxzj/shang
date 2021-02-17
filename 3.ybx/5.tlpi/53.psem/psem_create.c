#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <tlpi_hdr.h>


static void
usageError(const char *progName)
{
	fprintf(stderr, "Usage: %s [-cx] name [octal-perms [value]]\n", progName);
	fprintf(stderr, "    -c   Create semaphore (O_CREAT)\n");
	fprintf(stderr, "    -x   Create exclusively (O_EXCL)\n");
	exit(EXIT_FAILURE);
}



int main(int argc, char *argv[])
{
	int flags, opt;
	mode_t perms;
	unsigned int value;
	sem_t *sem;
	flags = 0;

	//printf("%d", optind);


	while ((opt = getopt(argc, argv, "cx")) != -1) {
		switch (opt) {
			case 'c':   flags |= O_CREAT;           break;
			case 'x':   flags |= O_EXCL;            break;
			default:    usageError(argv[0]);
		}
	}

	//printf("%d", optind);



	if (optind >= argc)
		usageError(argv[0]);

	/* Default permissions are rw-------; default semaphore initialization
	 *        value is 0 */
	if (argc <= optind + 1){
		perms = (S_IRUSR | S_IWUSR);
	}else{
		perms = getInt(argv[optind + 1], GN_BASE_8, "octal-perms");
	}

	value = (argc <= optind + 2) ? 0 : getInt(argv[optind + 2], 0, "value");

	sem = sem_open(argv[optind], flags, perms, value);
	if (sem == SEM_FAILED)
		errExit("sem_open");

	exit(EXIT_SUCCESS);



	return 0;
}
