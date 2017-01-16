#ifndef _EVENT3_EVENT_H_
#define _EVENT3_EVENT_H_ 

#include <event3/event-config.h>
#ifdef _EVENT_HAVE_SYS_TYPES_H 
#include <sys/types.h> 
#endif 
#ifdef _EVENT_HAVE_SYS_TIME_H 
#include <sys/time.h> 
#endif 
 
#include <stdio.h> 
 
#include <event3/util.h>

#define EVENT_LOG_DEBUG 0
#define EVENT_LOG_MSG   1
#define EVENT_LOG_WARN  2
#define EVENT_LOG_ERR   3

#define _EVENT_LOG_DEBUG EVENT_LOG_DEBUG
#define _EVENT_LOG_MSG EVENT_LOG_MSG
#define _EVENT_LOG_WARN EVENT_LOG_WARN
#define _EVENT_LOG_ERR EVENT_LOG_ERR

typedef void (*event_log_cb)(int severity, const char *msg);
void event_set_log_callback(event_log_cb cb);

typedef void (*event_fatal_cb)(int err);
void event_set_fatal_callback(event_fatal_cb cb);

#endif
