CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=main.o llist.o
TARGET=llist

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
