#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

static jmp_buf env;

//目的:证明如果在响应信号的函数中使用了longjmp，会使得这个信号在以后一直是阻塞状态
//办法:longjmp前一定要复原信号的mask位不要影响后续使用

//例1:如果不UN_BLOCK,最后只能Ctl+\
// ./a.out 
// ^Chandler start
// longjmp ret:123
// ^C^C^C^C^C^C^C^C^\退出 (核心已转储)

//例2:UN_BLOCK　可以看到后边SIGINT都正常响应了
//./a.out 
//^Chandler start
//longjmp ret:123
//^Chandler start
//longjmp ret:123
//^C^Chandler start
//longjmp ret:123
//^Chandler start
//longjmp ret:123
//^Chandler start
//longjmp ret:123
//^\退出 (核心已转储)

void handler(int s){
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	printf("handler start\n");
	sigprocmask(SIG_UNBLOCK, &set, NULL);//lopngjmp前一定要UN_BLOCK这个信号
	longjmp(env, 123);
	printf("handler will never complete!\n");
}

int main(int argc, const char *argv[])
{
	int ret;
	signal(SIGINT, handler);
	ret = setjmp(env);
	if (ret == 0) {

	}else{
		printf("longjmp ret:%d\n", ret);
	}
	pause();
	return 0;
}
