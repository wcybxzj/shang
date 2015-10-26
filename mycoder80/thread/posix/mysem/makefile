CFLAGS+=-pthread
LDFLAGS+=-pthread

all:mysem

mysem:mysem.o main.o
	gcc $^ -o $@  $(CFLAGS) $(LDFLAGS) -Wall

clean:
	rm *.o mysem -rf
