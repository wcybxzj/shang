PROGS = 00.sort_test 01.bubble_test 02.select_vs_bubble
OBJS =  1.bubble.o 2.select.o 3.insert.o 4.shell.o 5.heap.o 6.merge.o
CFLAGS=-g -g3 -gdwarf-2
$(CC)=gcc
all:$(PROGS)

00.sort_test:00.sort_test.o $(OBJS) func.o
	$(CC) $(CFLAGS) -o 00.sort_test 00.sort_test.o $(OBJS) func.o

01.bubble_test:01.bubble_test.o 1.bubble.o 2.select.o func.o
	$(CC) $(CFLAGS) -o 01.bubble_test 01.bubble_test.o 1.bubble.o  2.select.o func.o

02.select_vs_bubble:02.select_vs_bubble.o 1.bubble.o 2.select.o func.o
	$(CC) $(CFLAGS) -o 02.select_vs_bubble 02.select_vs_bubble.o 1.bubble.o  2.select.o func.o



clean:
	rm -f a.out *.o $(PROGS) core.*
