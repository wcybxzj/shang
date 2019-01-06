#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/*
说明:
waitpid的 WUNTRACED 和 WCONTINUED
让parent进程可以检测到子进程的被STOP或者CONTINUE的情况

代码来源:man waitpid
*/

/*
测试:
terminal 1:
./2_2.man_waitpid
Child PID is 8971
stopped by signal 19
continued
killed by signal 15

terminal 2:
kill -STOP 8971
kill -CONT 8971
kill -TERM 8971


terminal 3:
ps aux|grep 8971
root       8971  0.0  0.0   4508    68 pts/2    S+   20:01   0:00 ./2_2.man_waitpid

ps aux|grep 8971
root       8971  0.0  0.0   4508    68 pts/2    T+   20:01   0:00 ./2_2.man_waitpid

ps aux|grep 8971
root       8971  0.0  0.0   4508    68 pts/2    S+   20:01   0:00 ./2_2.man_waitpid
*/
int main(int argc, char *argv[])
{
	pid_t cpid, w;
	int wstatus;

	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {            /* Code executed by child */
		printf("Child PID is %ld\n", (long) getpid());
		if (argc == 1)
			pause();                    /* Wait for signals */
		_exit(atoi(argv[1]));

	} else {                    /* Code executed by parent */
		do {
			w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
			if (w == -1) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}

			if (WIFEXITED(wstatus)) {
				printf("exited, status=%d\n", WEXITSTATUS(wstatus));
			} else if (WIFSIGNALED(wstatus)) {
				printf("killed by signal %d\n", WTERMSIG(wstatus));
			} else if (WIFSTOPPED(wstatus)) {
				printf("stopped by signal %d\n", WSTOPSIG(wstatus));
			} else if (WIFCONTINUED(wstatus)) {
				printf("continued\n");
			}
		} while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
		exit(EXIT_SUCCESS);
	}
}

