# you have to use tabs NOT SPACES

#compiling gcc
#-Wall enables Warning message -> its compulsory
#-o changes name of the output file.
#-c -> without linking. You use it when you compile libraries. 
#Then you have compile .o files with linking

#you start your program by typing make in folder which contains Makefile file
#if you type only "make" the first section will start
#if you type for example "make exec" "exec" section will start

#you use standard linux commands in this file

#good way to create makefile

#all:
#	gcc -Wall function1.c function2.c function3.c main.c -o compiled.out
#exec:
#	./compiled.out
#clean:
#	rm -rf *.out		

#you can also create something like variable to make this code shorter

# CC = gcc -Wall
# all:
# 	$(CC) function1.c function2.c function3.c main.c -o compiled.out
# exec:
# 	./compiled.out
# clean:
# 	rm -rf *.out		

#its good idea to separate everything clean up etc

CC = gcc -Wall
all:
	make clean
	make out
	make exec
	make clean
	

compile:
	$(CC) -c function1.c
	$(CC) -c function2.c
	$(CC) -c function3.c
	$(CC) function1.o function2.o function3.o main.c -o main

exec:
	./main

clean: 
	rm -f *.o main



