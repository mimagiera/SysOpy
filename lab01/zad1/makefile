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
