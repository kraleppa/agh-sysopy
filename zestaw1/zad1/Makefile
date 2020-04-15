CC = gcc -Wall

all:
	make clear
	make compile
	make exec
	make clear


static:
	make clear
	$(CC) -c table.c
	ar crs libtable.a table.o
	$(CC) main.c -o main -L. -ltable

shared:
	make clear
	$(CC) -fPIC -shared table.c -o libtable.so
	$(CC) main.c -L. -Wl,-rpath=. -ltable -o main

exec:
	./main

clear:
	rm -f *.o *.a *.so tmp.txt main
	clear