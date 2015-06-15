C = gcc
CFLAGS = -Wall -g -std=c99 -pedantic -pthread
LIBS = ldrop

all: lib client  

client: main.c 
	$(CC) $(CFLAGS) -o $@ $^ -L../lib -ldrop
lib:
	$(MAKE) -C ../lib

clean:
	rm client client.data
