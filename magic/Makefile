#
#
#
#
#
OBJ=can.o paint.o pthread.o misc.o cursor.o score.o main.o
TARGET=main
CC=gcc
$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -lpthread
main.o:./src/main.c
	$(CC) -c $^
misc.o:./src/misc.c
	$(CC) -c $^
cursor.o:./src/cursor.c
	$(CC) -c $^
pthread.o:./src/pthread.c
	$(CC) -c $^
paint.o:./src/paint.c
	$(CC) -c $^
can.o:./src/can.c
	$(CC) -c $^
score.o:./src/score.c
	$(CC) -c $^
.PHONLY:clean
clean:
	rm -rf $(OBJ)
