#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"


void timeout_cb(int fd, short event, void *arg)  
{  
    printf("in the timeout_cb\n");  
}  

//需要Windows运行的出bug的效果,我linux所以这个代码只能看不能运行
  
//出现的bug:
//这个例子要运行在不支持monotonic时间的系统
//由于Libevent的解决方法并不是很精确，所以还是会有一些bug。
//下面给出一个bug。如果用户是在调用event_new函数之后，
//event_add之前对系统时间进行修改，那么无论用户设置的event超时有多长，都会马上触发超时。

//分析:
//这个bug的出现是因为，在event_base_new_with_config函数中有gettime(base,&base->event_tv)，所以event_tv记录了修改前的时间。
//而event_add是在修改系统时间后才调用的。
//所以event结构体的ev_timeout变量使用的是修改系统时间后的超时时间，这是正确的时间。
//在执行timeout_correct函数时，Libevent发现用户修改了系统时间，所以就将本来正确的ev_timeout减去了off。
//所以ev_timeout就变得比较修改后的系统时间小了。在后面检查超时时，就会发现该event已经超时了(实际是没有超时)，就把它触发。
//如果该event有EV_PERSIST属性，那么之后的超时则会是正确的。这个留给读者去分析吧。
 
//另外，Libevent并没有考虑把时钟往后调，比如现在是9点，用户把系统时间调成10点。
//上面的代码如果用户是在event_add之后修改系统时间，就能发现这个bug。

int main()  
{  
    struct event_base *base = event_base_new();  
  
    struct event *ev = event_new(base, -1, EV_TIMEOUT, timeout_cb, NULL);  
  
    int ch;  
	//暂停，让用户有时间修改系统时间。可以将系统时间往前1个小时  
    scanf("%c", &ch);   
  
    struct timeval tv = {100, 0};//这个超时时长要比较长。这里取100秒  
    //第二个参数不能为NULL.不然也是不能触发超时的。毕竟没有时间  
    event_add(ev, &tv);  
  
    event_base_dispatch(base);  
  
    return 0;  
}  
