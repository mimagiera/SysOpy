CC = gcc -Wall -std=c11 -ggdb

all: clean main

main:
	$(CC) -o trucker trucker.c -I .
	$(CC) -o loader loader.c -I .

clean:
	rm -f trucker loader