CC = gcc -Wall -std=c11 -ggdb

main:
	$(CC) -o monitor monitor.c -I .
	make test

test:
	./monitor "./lista" 10 0