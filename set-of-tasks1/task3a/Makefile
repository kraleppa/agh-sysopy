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
	rm -f *.o *.a *.so tmp.txt main
	clear

compare_test1:
	./main compare_pairs a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt a.txt:b.txt

compare_test2:
	./main compare_pairs loremA.txt:loremB.txt loremA.txt:loremB.txt loremA.txt:loremB.txt loremA.txt:loremB.txt loremA.txt:loremB.txt loremA.txt:loremB.txt

save_delete_test:
	./main save_tmp_file remove_block 0 save_tmp_file remove_block 1 save_tmp_file remove_block 2 save_tmp_file remove_block 3 save_tmp_file remove_block 4 save_tmp_file remove_block 5



