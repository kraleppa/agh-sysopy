CC = gcc -Wall

all:
	make dynamic
	make exec
	make clear

dynamic: 
	make clear
	#We create .o files
	$(CC) -c function1.c
	$(CC) -c function2.c
	$(CC) -c function3.c

	#now we create a library
		#-shared -fPIC - compulsory options for dynamic library
		#-o lib<libraryName>.so - our library file
		#then - .o files which we use to create a library
	
	gcc -Wall -shared -fPIC -o libfunctions.so function1.o function2.o function3.o

	#now you compile everything together
	#pwd -> linux command which gives you your current location
	#i dont know what -Wl and -rpath=`pwd` works, but U MNIE DZIALA
	# other options the same like in static libraries
	gcc main.c -Wall -o main -L. -lfunctions -Wl,-rpath=`pwd`

clear:
	rm -f *.o *.a *.so main

exec:
	./main

