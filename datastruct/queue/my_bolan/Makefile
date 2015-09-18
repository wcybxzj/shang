CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=nibolan.o llist.o linkqueue.o linkstack.o
TARGET=nibolan

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
