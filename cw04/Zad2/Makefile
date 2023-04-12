CC = gcc -Wall

all: clean main test

main:
	gcc -o main main.c

test:
	./main SA_SIGINFO
	./main SA_NOCLDSTOP
	./main SA_NOCLDWAIT


clean:
		rm -f *.o *.a a.out main