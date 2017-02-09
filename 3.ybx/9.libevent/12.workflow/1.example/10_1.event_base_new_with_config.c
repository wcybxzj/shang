#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"

void test_for_select()
{
    struct event_config *cfg = event_config_new();  
    event_config_require_features(cfg, EV_FEATURE_FDS);  //第二个参数对应select.c中 struct selectops的features
    struct event_base *base = event_base_new_with_config(cfg);  
	if( base == NULL )  
	{  
		printf("don't support this features\n");  
		base = event_base_new(); 
	}else{
		printf("select ok!");
	}
}

void test_for_epoll()
{
    struct event_config *cfg = event_config_new();  
    event_config_require_features(cfg,  EV_FEATURE_O1 | EV_FEATURE_FDS);  //epoll.c structg epolltops的features
    struct event_base *base = event_base_new_with_config(cfg);  
	//epoll支持EV_FEATURE_ET|EV_FEATURE_O1。base是NULL。
	if( base == NULL )  
	{  
		printf("don't support this features\n");  
		base = event_base_new();
	}else{
		printf("epoll ok!");
	}
}


int main()  
{  
	test_for_select();
	printf("=============================================\n");
	test_for_epoll();
	return 0;  
}  
