C = gcc
CFLAGS = -Wall -g -std=c99 -pedantic -pthread
LIBS = ldrop

all: clean client  

client: main.c 
	$(CC) $(CFLAGS) -o $@ $^ -L../lib -ldrop
clean:
	rm client 
