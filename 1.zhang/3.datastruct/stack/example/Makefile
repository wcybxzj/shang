CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=transform.o llist.o linkstack.o
TARGET=transform

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
