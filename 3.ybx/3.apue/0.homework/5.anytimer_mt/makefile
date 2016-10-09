CFLAGS+=-pthread
LDFLAGS+=-pthread

all:anytimer

anytimer:anytimer.o main.o
	gcc $^ -o $@  $(CFLAGS) $(LDFLAGS)

clean:
	rm *.o anytimer -rf


