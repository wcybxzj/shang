CFLAGS+=-pthread
LDFLAGS+=-pthread

all:mysem

mysem:mysem.o main.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o mysem


