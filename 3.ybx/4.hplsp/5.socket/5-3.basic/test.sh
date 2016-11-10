#!/bin/bash

#echo 512 > /proc/sys/net/ipv4/tcp_max_syn_backlog
echo 5 > /proc/sys/net/ipv4/tcp_max_syn_backlog
for i in {1..10}; do 
{ 
	#echo pid: $!;  
	./client 192.168.91.11 1234
} & 
done
