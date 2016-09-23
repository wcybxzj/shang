1.测试进程线程创建速度几乎一样,关闭代码中的sleep()也可以用pause()
[root@web11 23.process_vs_thread]# time ./1.fork
finish
real	0m0.139s
user	0m0.013s
sys	0m0.104s
[root@web11 23.process_vs_thread]# time ./2.pthread_create

real	0m0.108s
user	0m0.016s
sys	0m0.155s

2.测试进程线程创建的进程开销,代码中开启sleep()也可用pause(),进程是线程的4倍
free -m
创建1000个进程 130m
创建1000个线程 30m
