AUTOMAKE_OPTIONS=foreign #软件等级
SUBDIRS=src  #先扫描子目录
bin_PROGRAMS=hello #软件生成后的可执行文件名称
hello_SOURCES=main.c #当前目录源文件
hello_LDADD=src/libpro.a #静态连接方式 连接src下生成的libpro.a文件
LIBS = -l pthread -l event #因为我们项目中用到了libevent和pthread，这个是动态连接
