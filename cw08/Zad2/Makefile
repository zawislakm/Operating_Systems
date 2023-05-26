CC = gcc
CFLAGS = -Wall

all:  clean life test
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -lncursesw

test:
	./life 5
clean:
	rm -f life
