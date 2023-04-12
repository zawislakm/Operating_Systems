CC = gcc -Wall

all: clean main test

main:
	gcc -o main main.c

test:
	./main 0.05 20
	./main 0.01 100

clean:
	rm -f *.o *.a a.out main