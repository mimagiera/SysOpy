CC = gcc -Wall -std=c11 -ggdb

all: clean main test

main:
	$(CC) -o main main.c -I .

test:
	./main "list.txt"

clean:
	rm -f main