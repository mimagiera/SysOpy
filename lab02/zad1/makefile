CC = gcc -Wall -std=c11 -ggdb

all: static

static:
	$(CC) -o program main.c -I .
	make tests
	make clean

clean:
	rm -f dane dane2

tests:
	rm -f "wyniki.txt" "dane" "dane2"
	printf "   Real      User      System\n" > wyniki.txt
	./program "generate" "dane" 100 1
	./program "copy" "dane" "dane2" 4000 1 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 1 "lib"
	./program "sort" "dane" 4000 1 "sys"
	./program "sort" "dane2" 4000 1 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 4
	./program "copy" "dane" "dane2" 4000 4 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 4 "lib"
	./program "sort" "dane" 4000 4 "sys"
	./program "sort" "dane2" 4000 4 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 512
	./program "copy" "dane" "dane2" 4000 512 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 512 "lib"
	./program "sort" "dane" 4000 512 "sys"
	./program "sort" "dane2" 4000 512 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 1024
	./program "copy" "dane" "dane2" 4000 1024 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 1024 "lib"
	./program "sort" "dane" 4000 1024 "sys"
	./program "sort" "dane2" 4000 1024 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 4096
	./program "copy" "dane" "dane2" 4000 4096 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 4096 "lib"
	./program "sort" "dane" 4000 4096 "sys"
	./program "sort" "dane2" 4000 4096 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 8192
	./program "copy" "dane" "dane2" 4000 8192 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 4000 8192 "lib"
	./program "sort" "dane" 4000 8192 "sys"
	./program "sort" "dane2" 4000 8192 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 1
	./program "copy" "dane" "dane2" 6000 1 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 1 "lib"
	./program "sort" "dane" 6000 1 "sys"
	./program "sort" "dane2" 6000 1 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 4
	./program "copy" "dane" "dane2" 6000 4 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 4 "lib"
	./program "sort" "dane" 6000 4 "sys"
	./program "sort" "dane2" 6000 4 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 512
	./program "copy" "dane" "dane2" 6000 512 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 512 "lib"
	./program "sort" "dane" 6000 512 "sys"
	./program "sort" "dane2" 6000 512 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 1024
	./program "copy" "dane" "dane2" 6000 1024 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 1024 "lib"
	./program "sort" "dane" 6000 1024 "sys"
	./program "sort" "dane2" 6000 1024 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 4096
	./program "copy" "dane" "dane2" 6000 4096 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 4096 "lib"
	./program "sort" "dane" 6000 4096 "sys"
	./program "sort" "dane2" 6000 4096 "lib"

	rm -f "dane" "dane2"

	./program "generate" "dane" 100 8192
	./program "copy" "dane" "dane2" 6000 8192 "sys"
	rm -f dane2
	./program "copy" "dane" "dane2" 6000 8192 "lib"
	./program "sort" "dane" 6000 8192 "sys"
	./program "sort" "dane2" 6000 8192 "lib"