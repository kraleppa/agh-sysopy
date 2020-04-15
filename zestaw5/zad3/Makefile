 CC = gcc -Wall

compile:
	clear
	$(CC) main.c -o main
	$(CC) consumer.c -o consumer
	$(CC) producer.c -o producer	


exec:
	clear
	./main

clear:
	clear
	rm -f main consumer producer pipe out.txt