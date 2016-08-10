CFLAGS+=-pthread
LDFLAGS+=-pthread

all:relayer

relayer:relayer.o main.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)
	
clean:
	rm *.o relayer -rf
