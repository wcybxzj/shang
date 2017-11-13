#!/bin/bash
#可以将所有Redis的oom_adj设置为最低值或者稍小的值，降低被OOM killer杀掉的概率
for redis_pid in $(pgrep -f "redis-server")
do
	echo -17 > /proc/${redis_pid}/oom_adj
	echo ${redis_pid}
	cat /proc/${redis_pid}/oom_adj
done
