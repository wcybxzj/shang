#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, const char *argv[])
{
	int i;
	int len;
	pid_t pid;
	char * str[100] = {NULL};
	char *line;
	size_t num;
	char *ptr = NULL;
	char *tmp = NULL;
	while (1) {
		i=0;
		printf("[mysh-01]$");
		getline(&line, &num, stdin);

		tmp = strrchr(line, '\n');
		*tmp = '\0';

		str[i++] = strtok(line, " ");
		while (ptr = strtok(NULL, " ")) {
			str[i] = ptr;
			printf("%s\n", str[i]);
			i++;
		}

		pid = fork();
		if (pid<0) {
			perror("fork()");
			exit(1);
		}
		if (pid==0) {
			execvp(str[0], str);
			//exit(1);
		}
		wait(NULL);
	}
	return 0;
}
