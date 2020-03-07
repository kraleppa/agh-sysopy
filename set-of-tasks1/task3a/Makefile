CC = gcc -Wall

static:
	$(CC) -c table.c
	ar crs libtable.a table.o
	$(CC) main.c -o main -L. -ltable

shared:
	$(CC) -fPIC -shared table.c -o libtable.so
	$(CC) main.c -L. -Wl,-rpath=. -ltable -o main


dynamic:
	$(CC) -c -fPIC table.c
	$(CC) -shared -fPIC -o libtable.so table.o
	$(CC) -o main main_dynamic.c -Wl,--no-as-needed -ldl -I .


clear:
	rm -f *.o *.a *.so *.txt main
	clear

test:
	echo -e "aaa\nbbb ccc\nddd\neee\nhhh iii\n" > a.txt
	echo -e "jjj kkk\naaa\nfff ccc\neee\nbbb ggg" > b.txt
	./main compare_pairs a.txt:b.txt remove_block 0 compare_pairs a.txt:b.txt   
	./main compare_pairs a.txt:b.txt remove_block 0 compare_pairs a.txt:b.txt remove_block 1
	rm -f *.o *.a *.so *.txt main


