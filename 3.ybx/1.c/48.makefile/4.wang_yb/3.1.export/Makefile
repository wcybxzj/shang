#在本例子中 = 和 :=效果一样
#export VALUE1 := export.c    #用了 export, 此变量能够传递到 ./other/Makefile 中
#VALUE2 := no-export.c        #此变量不能传递到 ./other/Makefile 中

export VALUE1 = export.c    #用了 export, 此变量能够传递到 ./other/Makefile 中
VALUE2 = no-export.c        #此变量不能传递到 ./other/Makefile 中

all:
	@echo "主 Makefile begin"
	@cd ./other && make
	@echo "主 Makefile end"
