CC = gcc -Wall

all: static shared  dynamic

static:
		gcc -c libwc.c
		ar rcs libwc.a libwc.o
		gcc -o repl_static repl.c libwc.c


shared:
		gcc -c -fPIC libwc.c -o libwc.o
		gcc -shared -Wl,-soname,libwc.so -o libwc.so libwc.o
		gcc -c repl.c -o repl.o
		sudo cp libwc.so /usr/lib/
		sudo ldconfig
		gcc -o repl_shared repl.o -L. -lwc

dynamic:
		gcc -c -fPIC libwc.c -o libwc.o
		gcc -shared -Wl,-soname,libwc.so -o libwc.so libwc.o
		gcc -c repl_dynamic.c -o repl_dynamic.o
		gcc -o repl_dynamic repl_dynamic.o -L. -ldl -lwc

clean:
		rm -f *.o *.a *.so repl_static repl_shared  repl_dynamic
		sudo rm -f /usr/lib/libwc.so