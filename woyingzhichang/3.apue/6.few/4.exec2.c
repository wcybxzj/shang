#include <stdio.h>
#include <unistd.h>

/*
	4.exec函数族
	//定参,调用函数
	execl();
	execlp();
	execle();

	//变参,调用函数
	execv();
	execvp();
	execvpe();
*/

//变参
void exec_changed_argv(char *argv[])
{
	execv("/bin/ls", argv);
}

//定参
void exec_fixed_argv(char *argv[])
{
	//要识别多少要自己写
	//execl("/bin/ls", argv[0], argv[1],NULL);
	execl("/bin/ls","wwwwwwwwwwww", argv[1],NULL);//argv[0]随便写没关系
}

int main(int argc, char *argv[])
{
	//exec_changed_argv(argv);
	exec_fixed_argv(argv);
	return 0;
}
