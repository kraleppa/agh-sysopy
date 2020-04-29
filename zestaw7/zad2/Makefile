 CC = gcc -Wall 
 
compile:
	clear
	$(CC) reciever.c -o reciever -lrt -pthread
	$(CC) bagger.c -o bagger -lrt -pthread
	$(CC) sender.c -o sender -lrt -pthread
	$(CC) main.c -o main -lrt -pthread
clear:
	clear
	rm -f bagger reciever sender main
	
