CFLAGS = -Wall
CC = gcc

main: main.o

clean:
	rm -f main.o main

test:  main
	@echo -e "\n\nTESTING\n\n"
	./main ..

all: main test

compile:
	$(CC) $(CFLAGS) main.c -o main
