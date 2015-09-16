CFLAGS+=-I.

CC=gcc
RM=rm
OBJS=main.o seqlist.o seqstack.o
TARGET=seqstack

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET)
