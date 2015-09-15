CFLAGS+=-I.	
#头文件的路径
#LDFLAGS+=-lm   
#编译选项

.PHONY:clean
#说明clean是一个伪目标文件

# =相当与#define CC gcc
# +=追加
CC=gcc
RM=rm
TARGET=seqlist
OBJS=main.o
OBJS+=seqlist.o

$(TARGET):$(OBJS)
	@$(CC) -o $@ $^
#$@取得目标文件，:左边的
#$^取得所有的依赖文件
#$<取得第一个依赖文件

clean:
	@$(RM) $(OBJS) $(TARGET)	
