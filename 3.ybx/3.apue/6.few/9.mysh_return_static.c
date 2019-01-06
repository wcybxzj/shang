#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1024

#define DELIM " \n\t"

typedef struct _cmd_st {
	glob_t res;
} cmd_st;

static void prompt()
{
	printf("dabing@x220$");
}

//返回值为静态变量的parse
//返回指向函数内部静态变量的问题是
//第一次输入　ls没问题
//第二次输入  直接回车还当成ls执行
static cmd_st parse_return_static(char *str){
	static cmd_st cmd;
	char *tok;
	int i = 0;
	while (1) {
		if (i == 0) {
			tok = strtok(str, DELIM);
		}else{
			tok = strtok(NULL, DELIM);
		}
		if (tok == NULL) {
			break;
		}
		//printf("%s\n", tok);
		glob(tok, GLOB_NOCHECK|GLOB_APPEND*i, NULL, &cmd.res);
		i = 1;
	}
	return cmd;
}

int main(int argc, const char *argv[])
{
	pid_t pid;
	int i;
	char *str = NULL;
	size_t size = 0;
	cmd_st cmd;

	while (1) {
		prompt();

		getline(&str, &size, stdin);

		cmd = parse_return_static(str);

		for (i = 0; i < cmd.res.gl_pathc; i++) {
			printf("%s\n", cmd.res.gl_pathv[i]);
		}

		if (0) {//内部命令

		}else{//外部命令
			pid = fork();
			if (pid < 0) {
				perror("fork()");
				exit(1);
			}
			if (pid == 0) {
				execvp(cmd.res.gl_pathv[0], cmd.res.gl_pathv);
				perror("exec error");
			}
			wait(NULL);
		}
	}


	return 0;
}
