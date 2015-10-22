CFLAGS+=-pthread
LDFLAGS+=-pthread

all:mytbf

mytbf:mytbf.o main.o
	gcc $^ -o $@  $(CFLAGS) $(LDFLAGS) -Wall

clean:
	rm *.o mytbf -rf
