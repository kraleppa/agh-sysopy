CC = gcc -Wall

all:
	make static
	make exec
	make clear

#creating static library
static: 
	#first thing is to clean up
	make clear
	#then you create .o files by compiling functions
	$(CC) -c function1.c
	$(CC) -c function2.c
	$(CC) -c function3.c
	#you create a library. You have to name libraries like this: lib<libraryName>.a
	ar crs libfunctions.a function1.o function2.o function3.o
	
	#now you compile main files with your library
		#-L<path> option imforms in which directory compiler should search for libarary file
		#-l<libraryName> option is about added librarywhich library to add
	$(CC) main.c -o main -L. -lfunctions
clear:
	rm -f *.o *.a *.so main

exec:
	./main

