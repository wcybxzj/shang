CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=main.o llist.o queue.o
TARGET=queue

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
