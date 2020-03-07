CC = gcc -Wall

compile:
	make clear
	$(CC) -c table.c
	ar crs libtable.a table.o
	$(CC) main.c -o main -L. -ltable

clear:
	rm -f *.o *.a *.so main
	clear

exec:
	./main