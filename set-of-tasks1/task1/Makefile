CC = gcc -Wall

all:
	make clear
	make compile
	make exec
	make clear


compile:
	$(CC) -c table.c
	ar crs libtable.a table.o
	$(CC) main.c -o main -L. -ltable

exec:
	./main

clear:
	rm -f *.o *.a main