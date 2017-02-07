#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"

/*
/10_2.event_get_supported_methods 
select	
current method:	 select
*/
int main()  
{  
#ifdef WIN32  
    WSADATA wsa_data;  
    WSAStartup(0x0201, &wsa_data);  
#endif  
  
    const char** all_methods = event_get_supported_methods();  
  
    while( all_methods && *all_methods )  
    {  
	        printf("%s\t", *all_methods++);  
	}  
  
    printf("\n");  
  
    struct event_base *base = event_base_new();  
    if( base )  
        printf("current method:\t %s\n", event_base_get_method(base) );  
    else  
        printf("base == NULL\n");  
  
#ifdef WIN32  
    WSACleanup();  
#endif  
  
    return 0;  
}  
