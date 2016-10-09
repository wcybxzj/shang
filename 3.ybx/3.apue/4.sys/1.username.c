#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, const char *argv[])
{
	struct passwd *pd;

	if (argc < 2) {
		printf("lack of uid\n");
		exit(1);
	}

	pd = getpwuid(atoi(argv[1]));
	printf("username %s\n", pd->pw_name);

	return 0;
}

