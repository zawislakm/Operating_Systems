CC = gcc -Wall

all: reverse reverse1024

#make before make test
reverse1024:
	gcc -DREVERSE1024 -o reverse1024 reverse.c


reverse:
	gcc -o reverse reverse.c

test:
	./reverse big.txt out.txt > result_reverse.txt
	./reverse1024 big.txt out.txt > result_reverse1024.txt

clean:
	rm -f replace_lib a.out reverse reverse1024