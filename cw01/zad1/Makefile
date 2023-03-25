CC = gcc -Wall

all: static shared

static:
	gcc -c libwc.c
	ar rcs libwc.a libwc.o

shared:
	gcc -c -fPIC libwc.c -o libwc.o
	gcc -shared -Wl,-soname,libwc.so -o libwc.so libwc.o



clean:
	rm -f *.o *.gch *.a *.so *.exe test.in a.out

