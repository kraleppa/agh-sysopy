CC = gcc -Wall

static:
	make clear
	clear
	$(CC) -c table.c
	ar crs libtable.a table.o
	$(CC) main.c -o main -L. -ltable

clear:
	rm -f *.o *.a *.so tmp.txt main

exec:
	./main





