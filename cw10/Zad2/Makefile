CC=gcc
CFLAGS=-O3

all: clean client server


client: client.c
	$(CC) $(CFLAGS) client.c -o client -Repsonds.h -lrt -lpthread

server: server.c
	$(CC) $(CFLAGS) server.c -o server -lrt -lpthread

clean:
	rm -f client server a.out