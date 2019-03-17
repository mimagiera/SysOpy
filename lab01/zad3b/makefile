CC = gcc -Wall -std=c11 -ggdb

all: static shared clean

static:
	$(CC) -c lib.c
	ar rcs lib.a lib.o

shared:
	$(CC) -c -fPIC lib.c
	$(CC) -shared -fPIC -o lib.so lib.o

clean:         
	rm -f *.o
	rm -f *.so
	rm -f *.a

zad3_static:
	make static
	$(CC) -o test main.c lib.a -I . -O$(Olevel)
	make tests

zad3_dynamic:
	make shared
	$(CC) -o test main_dynamic.c -I . -ldl -O$(Olevel)
	make tests

zad3_shared:
	make shared
	$(CC) -o test main.c -L. lib.so -I . -O$(Olevel)
	make tests

tests:
	rm -f "raport3b_part.txt"
	rm -f "tmp"
	./test 2 "search" "/" "*" "tmp" "search" "/opt" "*" "tmp" "search" "." "*" "tmp"
	./test 2 "save" "10000" "save" "100000000" "save" "200000000"
	./test 2 "delete" "10000" "delete" "100000000" "delete" "200000000"
	./test 5 "add_and_delete" "5" "1000000"
	./test 10 "add_and_delete" "10" "1000000"
	./test 10 "add_and_delete" "10" "2000000"
	rm -f "tmp"
