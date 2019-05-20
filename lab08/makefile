CC = gcc -Wall -std=c11 -ggdb -lpthread -lm

all: clean main

main:
	$(CC) -o filter filter.c -I .
	$(CC) -o random_gen random_gen.c -I .

clean:
	rm -f filter random_gen

test:
	./random_gen
	./filter "8" "interleaved" "gator.ascii.pgm" "filter.txt" "filtered.pgm"