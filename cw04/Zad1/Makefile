CC = gcc -Wall

all: clean child fork exec test_fork test_exec

child:
	gcc -o exec_child exec_child.c

fork:
	gcc -o fork fork.c

exec:
	gcc -o exec exec.c

test_fork:
	./fork ignore
	./fork handler
	./fork mask
	./fork pending

test_exec:
	./exec ignore
	./exec mask
	./exec pending


clean:
		rm -f *.o *.a a.out fork exec_child exec