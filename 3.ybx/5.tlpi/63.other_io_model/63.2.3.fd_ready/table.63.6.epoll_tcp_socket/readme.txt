//表63-6
/*
./server 127.0.0.1
poll
poll return ret:1
POLLIN
block
epoll_wait:ret:1
EPOLLOUT
block
epoll_wait:ret:1
EPOLLIN
len:3
EPOLLOUT
block
epoll_wait:ret:1
EPOLLIN
len:0
EOF
EPOLLOUT
block

./client 127.0.0.1
sleep 10
write abc 
^Csig_hanler
*/
