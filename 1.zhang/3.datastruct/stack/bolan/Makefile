CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=my_bolan.o llist.o linkstack.o
TARGET=my_bolan

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
