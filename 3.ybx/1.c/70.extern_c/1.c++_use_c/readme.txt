1.problem:
make
g++    -c -o moduleB.o moduleB.cpp
gcc moduleA.c -c -Wall -g -o moduleA.o
g++ moduleB.o moduleA.o -o main
moduleB.o: In function `main':
moduleB.cpp:(.text+0x1a): undefined reference to `foo(int, int)'
collect2: ld returned 1 exit status
make: *** [main] Error 1

原因:
c++函数有重载所以foo函数被解析成foo(int, int)
moduleA.o是c模块
moduleB.o是c++模块
c++使用c的模块时候，c模块头部需要指出函数是c类型的,
否则按照c++的方式找不到函数
==================================================

2.ok:
__cplusplus:
g++带进来的宏

extern "C"{..}:
在c头文件中指定,让c++知道这是c 函数,避免找不到函数名

资料：
http://www.cnblogs.com/rollenholt/archive/2012/03/20/2409046.html
