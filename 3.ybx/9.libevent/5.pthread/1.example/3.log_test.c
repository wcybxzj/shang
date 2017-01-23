#include <stdio.h>
#include <errno.h>
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下

void log_func(int severity, const char *msg)
{
	printf("用户定义的logfunc:%s\n",msg);
	return;
}

void fatal_func(int err_no)
{
	printf("用户定义的fatal func:%d\n",err_no);
	return;
}

void log_test()
{
	//event_err(EINTR,"string:%s", "1111111");
	//event_warn(" string:%s", "2222222");
	//event_sock_err(EINTR, EINTR, "string:%s", "333333" );
	//event_sock_warn(EINTR, "string:%s", "4444444" );
	//event_errx( EINTR, "string:%s", "55555555" );
	//event_warnx("string:%s", "666666666" );
	//event_msgx("string:%s", "77777777" );
	_event_debugx("string:%s", "88888888" );
}

//测试libevnet 日志系统
int main(int argc, const char *argv[])
{
	event_set_fatal_callback(fatal_func);
	event_set_log_callback(log_func);
	log_test();
	return 0;
}
