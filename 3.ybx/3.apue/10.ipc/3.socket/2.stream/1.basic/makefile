PROGS=server client
all:$(PROGS)

server:server.o
	$(CC) -g  -o server server.o


client:client.o
	$(CC) -g  -o client client.o

%.o: %.c
	$(CC) -g -c $<

clean:
	rm -rf $(PROGS) *.o
