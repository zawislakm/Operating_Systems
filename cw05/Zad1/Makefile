CC = gcc -Wall

all: clean main test

main:
	gcc -o main main.c

test:
	./main date
	./main sender



clean:
		rm -f *.o *.a a.out main