#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

//./mysu username cat /home/username
int main(int argc, char *argv[])
{
	printf("parent:ruid  %d, euid %d\n", getuid(), geteuid());
	int ret;
	struct passwd *pwd_p;
	pid_t pid;
	if (argc < 3) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	pid = fork();
	if (pid<0) {
		perror("fork():");
		exit(1);
	}
	if (pid==0) {
		printf("child :ruid  %d, euid %d\n", getuid(), geteuid());
		pwd_p = getpwnam(argv[1]);
		printf("%d\n", pwd_p->pw_uid);
		ret = setuid(pwd_p->pw_uid);
		if (ret < 0) {
			perror("setuid");
			exit(123);
		}
		execvp(argv[2], argv+2);
		perror("exec():");
		exit(1);
	}

	wait(NULL);

	return 0;
}
