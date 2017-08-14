object = get.o sum.o val.o \
main.o
main:${object}
	gcc -o main ${object}
main.o:get.h sum.h
val.o:val.h
sum.o:val.h sum.h
get.o:get.h

#可以用.PHONY来声明clean这样的不生成可执行文件的伪目标。可以看下Makefile改造后的例子：
#其中cleanall 既删除main文件，也执行clean下的命令，因为cleanall依赖于 clean伪目标。
.PHONY: clean cleanall
clean:
	rm -rf *.o
cleanall:clean
	rm -rf main
