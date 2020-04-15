 CC = gcc -Wall

compile:
	clear
	$(CC) main.c -o main

exec:
	clear
	./main

clear:
	clear
	rm -f main dane.txt dane

show:
	cat dane.txt

copyy:
	rm -f copy
	touch copy
	rm -f dane
	./main generate dane 50000 8192
	