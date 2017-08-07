#· AUTOMAKE_OPTIONS 为设置Automake的选项。
#	由于GNU对自己发布的软件有严格的规范，比如必须附带许可证声明文件COPYING等，否则Automake执行时会报错。
#	Automake提供了3种软件等级：foreign、gnu和gnits，供用户选择，默认等级为gnu。本例使需用foreign等级，它只检测必须的文件。

#· bin_PROGRAMS 定义要产生的执行文件名。如果要产生多个执行文件，每个文件名用空格隔开。

#· hello_SOURCES 定义”hello”这个执行程序所需要的原始文件。
#	如果”hello”这个程序是由多个原始文件所产生的，则必须把它所用到的所有原始文件都列出来，并用空格隔开。
#	例如：若目标体”hello”需要”hello.c”、”hello.h”两个依赖文件，则定义hello_SOURCES=hello.c hello.h。


AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=hello
hello_SOURCES=hello.cpp hello.h
