CC = gcc -Wall

all: static shared dll

#make before make test

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

dll:
		gcc -c -fPIC libwc.c -o libwc.o
		gcc -shared -Wl,-soname,libwc.so -o libwc.so libwc.o
		gcc -c repl_dynamic.c -o repl_dynamic.o
		gcc -o repl_dynamic repl_dynamic.o -L. -ldl -lwc

test:
	gcc -O1 -o repl_staticO1 repl.c libwc.c
	gcc -O2 -o repl_staticO2 repl.c libwc.c
	gcc -O3 -o repl_staticO3 repl.c libwc.c

	gcc -O1 -o repl_sharedO1 repl.o -L. -lwc
	gcc -O2 -o repl_sharedO2 repl.o -L. -lwc
	gcc -O3 -o repl_sharedO3 repl.o -L. -lwc

	gcc -O1 -o repl_dynamicO1 repl_dynamic.o -L. -ldl -lwc
	gcc -O2 -o repl_dynamicO2 repl_dynamic.o -L. -ldl -lwc
	gcc -O3 -o repl_dynamicO3 repl_dynamic.o -L. -ldl -lwc

	./repl_static <test.txt> result_static.txt
	./repl_static <test.txt> result_staticO1.txt
	./repl_static <test.txt> result_staticO2.txt
	./repl_static <test.txt> result_staticO3.txt

	./repl_shared <test.txt> result_shared.txt
	./repl_shared <test.txt> result_sharedO1.txt
	./repl_shared <test.txt> result_sharedO2.txt
	./repl_shared <test.txt> result_sharedO3.txt

	./repl_dynamic <test.txt> result_dynamic.txt
	./repl_dynamic <test.txt> result_dynamicO1.txt
	./repl_dynamic <test.txt> result_dynamicO2.txt
	./repl_dynamic <test.txt> result_dynamicO3.txt


clean:
		rm -f *.o *.a *.so repl_static repl_shared  repl_dynamic repl_dynamicO1 repl_dynamicO2 repl_dynamicO3 repl_sharedO1 repl_sharedO2 repl_sharedO3 repl_staticO1 repl_staticO2 repl_staticO3
		sudo rm -f /usr/lib/libwc.so