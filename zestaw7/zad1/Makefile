 CC = gcc -Wall 
 
compile:
	clear
	$(CC) reciever.c -o reciever -lrt
	$(CC) bagger.c -o bagger -lrt
	$(CC) sender.c -o sender -lrt
	$(CC) main.c -o main -lrt
clear:
	clear
	rm -f bagger reciever sender main
	
